#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include <nlohmann/json.hpp>

#include "types/ShardTypes.h"
#include "types/TrafficReport.h"

class RaftLite;
struct RaftLiteDeleter {
    void operator()(RaftLite* ptr) const;
};

struct OpLogEntry {
    long index;
    long term;
    std::string op;        // "UPSERT"
    TrafficReport payload;
    int shard_id;
};

nlohmann::json opLogEntryToJson(const OpLogEntry& e);
OpLogEntry opLogEntryFromJson(const nlohmann::json& j);

class Replication {
public:
    using ApplyFn = std::function<void(const OpLogEntry&)>;
    Replication(const std::string& selfId, const ClusterConfig& cfg);
    void startLeader(ApplyFn apply);
    void startFollower(ApplyFn apply);
    void stop();

    long appendAndReplicate(const OpLogEntry& e); // leader path
    void applyRemote(const OpLogEntry& e);        // follower path
    const ClusterConfig& config() const { return cfg_; }
    // status
    long term() const;
    long lastIndex() const;
private:
    friend class RaftLite;

    std::string self_id_;
    ClusterConfig cfg_;
    ApplyFn apply_fn_;
    ApplyFn follower_apply_fn_;
    std::unique_ptr<RaftLite, RaftLiteDeleter> raft_;
    mutable std::mutex mtx_;
    std::vector<OpLogEntry> log_;
};
