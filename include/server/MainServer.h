#pragma once
#include <atomic>
#include <thread>

#include "shard/ShardRouter.h"
#include "repl/Replication.h"
#include "agg/Aggregator.h"
#include "net/HttpServer.h"
#include "server/Status.h"
#include "types/ShardTypes.h"

class MainServer {
public:
    MainServer(const std::string& selfId, const ClusterConfig& cfg,
               const std::string& host, int httpPort, bool asLeader);
    void start();
    void stop();
private:
    void serveHttp_();          // /ingest, /ingest_internal, /data, /summary, /status
    void aggLoop_();            // recompute every 1s
    OpLogEntry makeEntry_(const TrafficReport& r, int shardId);
    bool isLeaderFor(int shardId) const;
    bool serveStaticFile_(const std::string& relPath, httplib::Response& res) const;
    static std::string guessMime_(const std::string& path);

    std::string selfId_;
    ClusterConfig cfg_;
    bool asLeader_{false};
    std::string host_;
    int httpPort_{0};
    ShardRouter router_;
    Replication repl_;
    HttpServer http_;
    Aggregator agg_;
    std::atomic<bool> running_{false};
    std::thread aggThread_;
    std::string staticRoot_{"dashboard"};
};
