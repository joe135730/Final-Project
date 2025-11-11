#include "server/MainServer.h"

#include <chrono>
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
            res.status = 200;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(std::string(R"({"error":")") + ex.what() + "\"}", "application/json");
        }
    });

    http_.Post("/ingest_internal", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = nlohmann::json::parse(req.body);
            auto entry = opLogEntryFromJson(body);
            repl_.applyRemote(entry);
            agg_.add(entry.payload);
            res.status = 200;
            res.set_content(R"({"status":"ok"})", "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(std::string(R"({"error":")") + ex.what() + "\"}", "application/json");
        }
    });

    http_.Get("/data", [this](const httplib::Request& req, httplib::Response& res) {
        auto road = req.get_param_value("road");
        RoadSnapshot snap;
        if (!agg_.get(road, snap)) {
            res.status = 404;
            res.set_content(R"({"error":"road not found"})", "application/json");
            return;
        }
        res.status = 200;
        res.set_content(snap.toJson().dump(), "application/json");
    });

    http_.Get("/summary", [this](const httplib::Request&, httplib::Response& res) {
        auto list = agg_.summary();
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& s : list) {
            arr.push_back(s.toJson());
        }
        res.status = 200;
        res.set_content(arr.dump(), "application/json");
    });

    http_.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
        StatusResponse st;
        st.term = repl_.term();
        st.last_index = repl_.lastIndex();
        st.summary = agg_.summary();
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


