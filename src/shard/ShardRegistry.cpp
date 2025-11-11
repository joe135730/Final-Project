#include "shard/ShardRegistry.h"

ShardRegistry::ShardRegistry(const ClusterConfig& cfg)
    : cfg_(cfg) {}

const Node* ShardRegistry::node(const std::string& id) const {
    return cfg_.findNode(id);
}

const ShardInfo* ShardRegistry::shard(int id) const {
    auto it = cfg_.shards.find(id);
    if (it == cfg_.shards.end()) return nullptr;
    return &it->second;
}


