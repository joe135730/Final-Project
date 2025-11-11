#include "server/MainServer.h"

#include <chrono>
#include <cctype>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <nlohmann/json.hpp>

#include "util/Logger.h"
#include "util/Time.h"
#include "net/HttpClient.h"

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

    serveHttp_();
    aggThread_ = std::thread([this]() { aggLoop_(); });
}

void MainServer::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    http_.stop();
    if (aggThread_.joinable()) {
        aggThread_.join();
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
                    res.status = 502;
                    res.set_content(R"({"error":"leader unavailable"})", "application/json");
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

    if (!http_.start(host_, httpPort_)) {
        throw std::runtime_error("HTTP server already running");
    }
}

void MainServer::aggLoop_() {
    while (running_.load()) {
        long now = nowMs();
        agg_.recompute(now);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
    return it->second.leader == selfId_ && asLeader_;
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


