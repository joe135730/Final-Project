#include "repl/Replication.h"

#include <sstream>
#include <stdexcept>
#include <utility>

#include "net/HttpClient.h"
#include "util/Logger.h"
#include "repl/RaftLite.h"

nlohmann::json opLogEntryToJson(const OpLogEntry& e) {
    return {
        {"index", e.index},
        {"term", e.term},
        {"op", e.op},
        {"shard_id", e.shard_id},
        {"payload", e.payload.toJson()}
    };
}

OpLogEntry opLogEntryFromJson(const nlohmann::json& j) {
    OpLogEntry e;
    e.index = j.at("index").get<long>();
    e.term = j.at("term").get<long>();
    e.op = j.at("op").get<std::string>();
    e.shard_id = j.at("shard_id").get<int>();
    e.payload = TrafficReport::fromJson(j.at("payload"));
    return e;
}

Replication::Replication(const std::string& selfId, const ClusterConfig& cfg)
    : self_id_(selfId),
      cfg_(cfg),
      raft_(std::make_unique<RaftLite>(*this)) {}

void Replication::startLeader(ApplyFn apply) {
    apply_fn_ = std::move(apply);
    raft_->startLeader();
}

void Replication::startFollower(ApplyFn apply) {
    follower_apply_fn_ = std::move(apply);
    raft_->startFollower();
}

void Replication::stop() {
    if (raft_) {
        raft_->stop();
    }
}

long Replication::appendAndReplicate(const OpLogEntry& e) {
    if (!apply_fn_) {
        throw std::runtime_error("Leader apply function not registered");
    }
    OpLogEntry entry = e;
    entry.op = "UPSERT";
    entry.term = raft_->term();
    entry.index = raft_->appendAndBroadcast(entry);
    {
        std::lock_guard<std::mutex> g(mtx_);
        log_.push_back(entry);
    }
    apply_fn_(entry);

    auto shardIt = cfg_.shards.find(entry.shard_id);
    if (shardIt != cfg_.shards.end()) {
        for (const auto& followerId : shardIt->second.followers) {
            if (followerId == self_id_) continue;
            auto node = cfg_.findNode(followerId);
            if (!node) continue;
            try {
                HttpClient cli(node->host, node->http_port);
                nlohmann::json resp;
                if (!cli.postJson("/ingest_internal", opLogEntryToJson(entry), &resp)) {
                    Logger::instance().warn("Replication failed to follower " + followerId);
                }
            } catch (const std::exception& ex) {
                Logger::instance().warn("Replication exception to follower " + followerId + ": " + ex.what());
            }
        }
    }

    return entry.index;
}

void Replication::applyRemote(const OpLogEntry& e) {
    if (!follower_apply_fn_) {
        throw std::runtime_error("Follower apply function not registered");
    }
    follower_apply_fn_(e);
    if (raft_) {
        raft_->applyRemote(e);
    }
    std::lock_guard<std::mutex> g(mtx_);
    log_.push_back(e);
}

long Replication::term() const {
    return raft_->term();
}

long Replication::lastIndex() const {
    return raft_->lastIndex();
}


