#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct Node {
    std::string id;
    std::string host;
    int http_port{0};
};

struct ShardInfo {
    int id{0};
    std::string leader;
    std::vector<std::string> followers;
};

using NodeMap = std::unordered_map<std::string, Node>;
using ShardMap = std::unordered_map<int, ShardInfo>;

struct ClusterConfig {
    int replication_factor{1};
    int partitions{1};
    NodeMap nodes;
    ShardMap shards;

    const Node* findNode(const std::string& id) const;
};

inline const Node* ClusterConfig::findNode(const std::string& id) const {
    auto it = nodes.find(id);
    if (it == nodes.end()) return nullptr;
    return &it->second;
}



