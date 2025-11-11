#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "types/ShardTypes.h"

namespace JsonIO {

ClusterConfig loadClusterConfig(const std::string& path);
std::vector<std::string> loadRoadList(const std::string& path);
void writeJson(const std::string& path, const nlohmann::json& j);

}


