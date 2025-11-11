#pragma once

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

struct RoadSnapshot {
    std::string road;
    std::int64_t last_report_ms{0};
    int cars_5s{0};
    int cars_60s{0};
    double avg_speed_kmh{0.0};
    double ewma_speed_kmh{0.0};
    std::string classification;

    nlohmann::json toJson() const;
};


