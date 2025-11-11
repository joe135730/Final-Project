#include "shard/ShardRouter.h"

#include <stdexcept>

ShardRouter::ShardRouter(const ClusterConfig& cfg)
    : cfg_(cfg) {
    if (cfg_.partitions <= 0) {
        throw std::runtime_error("Partitions must be > 0");
    }
}

int ShardRouter::shardFor(const std::string& road) const {
    std::hash<std::string> h;
    return static_cast<int>(h(road) % cfg_.partitions);
}

const Node& ShardRouter::leaderFor(int shardId) const {
    auto it = cfg_.shards.find(shardId);
    if (it == cfg_.shards.end()) {
        throw std::runtime_error("Unknown shard id: " + std::to_string(shardId));
    }
    const auto& leaderId = it->second.leader;
    auto node = cfg_.findNode(leaderId);
    if (!node) {
        throw std::runtime_error("Leader node not found: " + leaderId);
    }
    return *node;
}

const Node& ShardRouter::followerFor(int shardId) const {
    auto it = cfg_.shards.find(shardId);
    if (it == cfg_.shards.end()) {
        throw std::runtime_error("Unknown shard id: " + std::to_string(shardId));
    }
    if (it->second.followers.empty()) {
        throw std::runtime_error("No followers configured for shard: " + std::to_string(shardId));
    }
    const auto& followerId = it->second.followers.front();
    auto node = cfg_.findNode(followerId);
    if (!node) {
        throw std::runtime_error("Follower node not found: " + followerId);
    }
    return *node;
}


