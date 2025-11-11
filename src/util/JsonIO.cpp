#include "util/JsonIO.h"

#include <fstream>
#include <stdexcept>
#include <vector>

#include "util/Logger.h"

namespace {
nlohmann::json readJsonFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Unable to open JSON file: " + path);
    }
    nlohmann::json j;
    in >> j;
    return j;
}
}

namespace JsonIO {

ClusterConfig loadClusterConfig(const std::string& path) {
    auto j = readJsonFile(path);
    ClusterConfig cfg;
    cfg.replication_factor = j.value("replication_factor", 1);
    cfg.partitions = j.value("partitions", 1);
    for (const auto& node : j["nodes"]) {
        Node n;
        n.id = node.at("id").get<std::string>();
        n.host = node.at("host").get<std::string>();
        n.http_port = node.at("http").get<int>();
        cfg.nodes[n.id] = n;
    }
    for (const auto& shard : j["shards"]) {
        ShardInfo s;
        s.id = shard.at("id").get<int>();
        s.leader = shard.at("leader").get<std::string>();
        if (shard.contains("followers")) {
            s.followers = shard.at("followers").get<std::vector<std::string>>();
        }
        cfg.shards[s.id] = s;
    }
    return cfg;
}

std::vector<std::string> loadRoadList(const std::string& path) {
    auto j = readJsonFile(path);
    if (!j.is_array()) {
        throw std::runtime_error("roads.json must be an array of strings");
    }
    return j.get<std::vector<std::string>>();
}

void writeJson(const std::string& path, const nlohmann::json& j) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Unable to open JSON file for writing: " + path);
    }
    out << j.dump(2);
}

}


