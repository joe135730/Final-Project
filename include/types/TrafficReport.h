#pragma once

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

struct TrafficReport {
    std::string road;
    std::int64_t timestamp_ms{0};
    int vehicle_count{0};
    double avg_speed_kmh{0.0};

    bool valid() const;
    nlohmann::json toJson() const;
    static TrafficReport fromJson(const nlohmann::json& j);
};


