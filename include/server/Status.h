#pragma once

#include <nlohmann/json.hpp>
#include <vector>

#include "types/RoadSnapshot.h"

struct StatusResponse {
    long term{0};
    long last_index{0};
    std::vector<RoadSnapshot> summary;

    nlohmann::json toJson() const;
};


