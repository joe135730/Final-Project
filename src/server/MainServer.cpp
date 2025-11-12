#include "server/MainServer.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "util/Logger.h"
#include "util/Time.h"
#include "net/HttpClient.h"
#include "util/JsonIO.h"

MainServer::MainServer(const std::string& selfId,
                       const ClusterConfig& cfg,
                       const std::string& host,
                       int httpPort,
                       bool asLeader)
    : selfId_(selfId),
      cfg_(cfg),
      asLeader_(asLeader),
      host_(host),
      httpPort_(httpPort),
      router_(cfg),
      repl_(selfId, cfg) {}

namespace {
bool hasLeadership(const ClusterConfig& cfg, const std::string& nodeId) {
    for (ShardMap::const_iterator it = cfg.shards.begin();
         it != cfg.shards.end(); ++it) {
        if (it->second.leader == nodeId) {
            return true;
        }
    }
    return false;
}
}

void MainServer::start() {
    if (running_.exchange(true)) {
        return;
    }

    if (!asLeader_) {
        asLeader_ = hasLeadership(cfg_, selfId_);
    }

    if (asLeader_) {
        repl_.startLeader([this](const OpLogEntry& e) {
            agg_.add(e.payload);
        });
    } else {
        repl_.startFollower([this](const OpLogEntry& e) {
            agg_.add(e.payload);
        });
    }

    // Seed aggregator with all known roads from config
    try {
        auto roads = JsonIO::loadRoadList("config/roads.json");
        for (const auto& road : roads) {
            agg_.seedRoad(road);
        }
        Logger::instance().info("Seeded " + std::to_string(roads.size()) + " roads into aggregator");
    } catch (const std::exception& ex) {
        Logger::instance().warn("Failed to seed roads: " + std::string(ex.what()));
    }

    serveHttp_();
    aggThread_ = std::thread([this]() { aggLoop_(); });
    failoverThread_ = std::thread([this]() { failoverLoop_(); });
}

void MainServer::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    http_.stop();
    if (aggThread_.joinable()) {
        aggThread_.join();
    }
    if (failoverThread_.joinable()) {
        failoverThread_.join();
    }
    repl_.stop();
}

void MainServer::serveHttp_() {
    auto addCors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    http_.Options(R"(/.*)", [addCors](const httplib::Request&, httplib::Response& res) {
        addCors(res);
        res.status = 200;
    });

    http_.Get("/", [this, addCors](const httplib::Request&, httplib::Response& res) {
        addCors(res);
        if (!serveStaticFile_(staticRoot_ + "/index.html", res)) {
            res.status = 404;
            res.set_content("Dashboard not found", "text/plain");
        }
    });

    http_.Get(R"(/static/(.*))", [this, addCors](const httplib::Request& req, httplib::Response& res) {
        addCors(res);
        if (req.matches.size() < 2) {
            res.status = 404;
            res.set_content("Not found", "text/plain");
            return;
        }
        std::string rel = req.matches[1];
        if (rel.find("..") != std::string::npos) {
            res.status = 403;
            res.set_content("Forbidden", "text/plain");
            return;
        }
        std::string path = staticRoot_ + "/static/" + rel;
        if (!serveStaticFile_(path, res)) {
            res.status = 404;
            res.set_content("Not found", "text/plain");
        }
    });

    http_.Post("/ingest", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = nlohmann::json::parse(req.body);
            TrafficReport report = TrafficReport::fromJson(body);
            if (report.timestamp_ms == 0) {
                report.timestamp_ms = nowMs();
            }
            int shardId = router_.shardFor(report.road);
            if (!isLeaderFor(shardId)) {
                const auto& leaderId = cfg_.shards.at(shardId).leader;
                auto node = cfg_.findNode(leaderId);
                if (!node) {
                    res.status = 500;
                    res.set_content(R"({"error":"leader not found"})", "application/json");
                    return;
                }
                nlohmann::json forward = report.toJson();
                HttpClient cli(node->host, node->http_port);
                nlohmann::json resp;
                if (!cli.postJson("/ingest", forward, &resp)) {
                    // Leader is unavailable - check if we can promote to leader
                    if (isFollowerFor(shardId)) {
                        Logger::instance().info("Leader " + leaderId + " unavailable for shard " + 
                                                std::to_string(shardId) + ", promoting " + selfId_ + " to leader");
                        promoteToLeaderFor(shardId);
                        // Now process as leader
                        auto entry = makeEntry_(report, shardId);
                        long idx = repl_.appendAndReplicate(entry);
                        nlohmann::json response{
                            {"status", "ok"},
                            {"index", idx},
                            {"term", repl_.term()},
                            {"promoted", true}
                        };
                        res.set_header("Access-Control-Allow-Origin", "*");
                        res.status = 200;
                        res.set_content(response.dump(), "application/json");
                        return;
                    } else {
                        res.status = 502;
                        res.set_content(R"({"error":"leader unavailable"})", "application/json");
                        return;
                    }
                } else {
                    res.status = 200;
                    res.set_content(resp.dump(), "application/json");
                }
                return;
            }

            auto entry = makeEntry_(report, shardId);
            long idx = repl_.appendAndReplicate(entry);
            nlohmann::json response{
                {"status", "ok"},
                {"index", idx},
                {"term", repl_.term()}
            };
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 200;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(std::string(R"({"error":")") + ex.what() + "\"}", "application/json");
        }
    });

    http_.Post("/ingest_internal", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = nlohmann::json::parse(req.body);
            auto entry = opLogEntryFromJson(body);
            repl_.applyRemote(entry);
            agg_.add(entry.payload);
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 200;
            res.set_content(R"({"status":"ok"})", "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(std::string(R"({"error":")") + ex.what() + "\"}", "application/json");
        }
    });

    http_.Get("/data", [this](const httplib::Request& req, httplib::Response& res) {
        auto road = req.get_param_value("road");
        RoadSnapshot snap;
        if (!agg_.get(road, snap)) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 404;
            res.set_content(R"({"error":"road not found"})", "application/json");
            return;
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 200;
        res.set_content(snap.toJson().dump(), "application/json");
    });

    http_.Get("/summary", [this](const httplib::Request&, httplib::Response& res) {
        auto list = agg_.summary();
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& s : list) {
            arr.push_back(s.toJson());
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 200;
        res.set_content(arr.dump(), "application/json");
    });

    http_.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
        StatusResponse st;
        st.term = repl_.term();
        st.last_index = repl_.lastIndex();
        st.summary = agg_.summary();
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 200;
        res.set_content(st.toJson().dump(), "application/json");
    });

    // Sync endpoint: followers can request full data snapshot from leader
    http_.Get("/sync", [this](const httplib::Request&, httplib::Response& res) {
        auto list = agg_.summary(1000); // Get all roads, not just top 20
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& s : list) {
            arr.push_back(s.toJson());
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 200;
        res.set_content(arr.dump(), "application/json");
    });

    // Aggregated summary: merge data from all peers for unified dashboard view
    http_.Get("/summary_all", [this](const httplib::Request&, httplib::Response& res) {
        std::unordered_map<std::string, RoadSnapshot> merged;
        
        // Start with local data
        auto localList = agg_.summary(1000);
        for (const auto& snap : localList) {
            merged[snap.road] = snap;
        }
        
        // Query all other nodes in parallel
        std::vector<std::thread> threads;
        std::mutex mergeMtx;
        
        for (const auto& nodePair : cfg_.nodes) {
            const auto& node = nodePair.second;
            if (node.id == selfId_) continue; // Skip self
            
            threads.emplace_back([&node, &merged, &mergeMtx]() {
                try {
                    HttpClient cli(node.host, node.http_port);
                    nlohmann::json peerData;
                    if (cli.getJson("/summary", &peerData, 1000)) {
                        if (peerData.is_array()) {
                            std::lock_guard<std::mutex> lock(mergeMtx);
                            for (const auto& snapJson : peerData) {
                                try {
                                    RoadSnapshot snap = RoadSnapshot::fromJson(snapJson);
                                    // Merge: prefer data with more recent timestamp or non-UNKNOWN classification
                                    auto it = merged.find(snap.road);
                                    if (it == merged.end()) {
                                        merged[snap.road] = snap;
                                    } else {
                                        // Prefer snapshot with data over UNKNOWN
                                        if (snap.classification != "UNKNOWN" && 
                                            it->second.classification == "UNKNOWN") {
                                            it->second = snap;
                                        } else if (snap.last_report_ms > it->second.last_report_ms) {
                                            // Prefer more recent data
                                            it->second = snap;
                                        }
                                    }
                                } catch (const std::exception& ex) {
                                    // Skip invalid entries
                                }
                            }
                        }
                    }
                } catch (const std::exception& ex) {
                    // Node unavailable, skip
                }
            });
        }
        
        // Wait for all threads
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        
        // Convert merged map to array
        std::vector<RoadSnapshot> result;
        result.reserve(merged.size());
        for (const auto& kv : merged) {
            result.push_back(kv.second);
        }
        
        // Sort by classification priority, then by traffic
        std::sort(result.begin(), result.end(), [](const RoadSnapshot& a, const RoadSnapshot& b) {
            if (a.classification == b.classification) {
                return a.cars_60s > b.cars_60s;
            }
            auto rank = [](const std::string& cls) {
                if (cls == "CONGESTED") return 0;
                if (cls == "MODERATE") return 1;
                if (cls == "SMOOTH") return 2;
                return 3; // UNKNOWN/STALE
            };
            return rank(a.classification) < rank(b.classification);
        });
        
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& s : result) {
            arr.push_back(s.toJson());
        }
        
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 200;
        res.set_content(arr.dump(), "application/json");
    });

    if (!http_.start(host_, httpPort_)) {
        throw std::runtime_error("HTTP server already running");
    }
    // Give HTTP server time to bind to port before accepting connections
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Logger::instance().info("HTTP server ready on " + host_ + ":" + std::to_string(httpPort_));
}

void MainServer::aggLoop_() {
    while (running_.load()) {
        long now = nowMs();
        agg_.recompute(now);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void MainServer::failoverLoop_() {
    // Check every 5 seconds for leader failures
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if (!running_.load()) break;
        
        // Check each shard where we're a follower
        for (const auto& shardPair : cfg_.shards) {
            int shardId = shardPair.first;
            const auto& shard = shardPair.second;
            
            // Skip if we're already the leader for this shard
            if (isLeaderFor(shardId)) {
                continue;
            }
            
            // Check if we're a follower for this shard
            if (!isFollowerFor(shardId)) {
                continue;
            }
            
            // Check if the configured leader is reachable
            const std::string& leaderId = shard.leader;
            auto leaderNode = cfg_.findNode(leaderId);
            if (!leaderNode) {
                continue;
            }
            
            // Try to ping the leader
            bool leaderAlive = false;
            try {
                HttpClient cli(leaderNode->host, leaderNode->http_port);
                nlohmann::json resp;
                if (cli.getJson("/status", &resp, 2000)) {
                    leaderAlive = true;
                }
            } catch (const std::exception& ex) {
                // Leader is down
            }
            
            // If leader is down and we're a follower, promote ourselves
            if (!leaderAlive) {
                Logger::instance().info("Leader " + leaderId + " for shard " + 
                                       std::to_string(shardId) + " appears down, promoting " + 
                                       selfId_ + " to leader");
                promoteToLeaderFor(shardId);
            }
        }
    }
}

OpLogEntry MainServer::makeEntry_(const TrafficReport& r, int shardId) {
    OpLogEntry entry;
    entry.payload = r;
    entry.shard_id = shardId;
    entry.index = 0;
    entry.term = 0;
    entry.op = "UPSERT";
    return entry;
}

bool MainServer::isLeaderFor(int shardId) const {
    auto it = cfg_.shards.find(shardId);
    if (it == cfg_.shards.end()) return false;
    // Check if we're the configured leader and in leader mode
    if (it->second.leader == selfId_ && asLeader_) {
        return true;
    }
    // Check if we've been promoted to leader and are a follower for this shard
    if (asLeader_ && isFollowerFor(shardId)) {
        return true;
    }
    return false;
}

bool MainServer::isFollowerFor(int shardId) const {
    auto it = cfg_.shards.find(shardId);
    if (it == cfg_.shards.end()) return false;
    const auto& followers = it->second.followers;
    return std::find(followers.begin(), followers.end(), selfId_) != followers.end();
}

void MainServer::promoteToLeaderFor(int shardId) {
    // Check if we're already leader
    if (isLeaderFor(shardId)) {
        return;
    }
    
    // Check if we're a follower for this shard
    if (!isFollowerFor(shardId)) {
        return;
    }
    
    // Before promoting, try to sync missing data from the original leader
    const auto& leaderId = cfg_.shards.at(shardId).leader;
    auto leaderNode = cfg_.findNode(leaderId);
    if (leaderNode && leaderId != selfId_) {
        // Try to sync data from the original leader
        try {
            HttpClient cli(leaderNode->host, leaderNode->http_port);
            nlohmann::json syncData;
            if (cli.getJson("/sync", &syncData, 2000)) {
                // Update aggregator with synced data
                if (syncData.is_array()) {
                    int syncedCount = 0;
                    for (const auto& snapJson : syncData) {
                        try {
                            RoadSnapshot snap = RoadSnapshot::fromJson(snapJson);
                            // Add to aggregator by creating a TrafficReport from snapshot
                            TrafficReport report;
                            report.road = snap.road;
                            report.timestamp_ms = snap.last_report_ms;
                            report.vehicle_count = snap.cars_5s > 0 ? snap.cars_5s : snap.cars_60s;
                            report.avg_speed_kmh = snap.avg_speed_kmh;
                            agg_.add(report);
                            syncedCount++;
                        } catch (const std::exception& ex) {
                            Logger::instance().warn("Failed to sync road snapshot: " + std::string(ex.what()));
                        }
                    }
                    Logger::instance().info("Synced " + std::to_string(syncedCount) + " roads from leader " + leaderId);
                }
            }
        } catch (const std::exception& ex) {
            Logger::instance().warn("Failed to sync from leader " + leaderId + ": " + ex.what());
        }
    }
    
    // Promote to leader mode if not already
    if (!asLeader_) {
        asLeader_ = true;
        // Switch replication from follower to leader
        repl_.stop();
        repl_.startLeader([this](const OpLogEntry& e) {
            agg_.add(e.payload);
        });
        Logger::instance().info("Promoted " + selfId_ + " to leader mode");
    }
}

bool MainServer::serveStaticFile_(const std::string& relPath, httplib::Response& res) const {
    std::ifstream in(relPath.c_str(), std::ios::binary);
    if (!in) {
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    std::string body = ss.str();
    std::string mime = guessMime_(relPath);
    res.status = 200;
    res.set_content(body, mime.c_str());
    return true;
}

std::string MainServer::guessMime_(const std::string& path) {
    static std::map<std::string, std::string> table;
    if (table.empty()) {
        table["html"] = "text/html; charset=utf-8";
        table["htm"] = "text/html; charset=utf-8";
        table["js"] = "application/javascript; charset=utf-8";
        table["css"] = "text/css; charset=utf-8";
        table["json"] = "application/json; charset=utf-8";
        table["png"] = "image/png";
        table["jpg"] = "image/jpeg";
        table["jpeg"] = "image/jpeg";
        table["svg"] = "image/svg+xml";
        table["txt"] = "text/plain; charset=utf-8";
    }
    std::string ext;
    size_t dot = path.find_last_of('.');
    if (dot != std::string::npos && dot + 1 < path.size()) {
        ext = path.substr(dot + 1);
        for (size_t i = 0; i < ext.size(); ++i) {
            ext[i] = static_cast<char>(std::tolower(ext[i]));
        }
    }
    std::map<std::string, std::string>::const_iterator it = table.find(ext);
    if (it != table.end()) {
        return it->second;
    }
    return "application/octet-stream";
}


