#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

#include "types/ShardTypes.h"

class ShardRegistry {
public:
    explicit ShardRegistry(const ClusterConfig& cfg);

    const Node* node(const std::string& id) const;
    const ShardInfo* shard(int id) const;

private:
    ClusterConfig cfg_;
};


