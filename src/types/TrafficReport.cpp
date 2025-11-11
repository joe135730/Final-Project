#include "types/TrafficReport.h"

#include <stdexcept>

using nlohmann::json;

bool TrafficReport::valid() const {
    return !road.empty() && vehicle_count >= 0 && avg_speed_kmh >= 0.0;
}

json TrafficReport::toJson() const {
    return json{
        {"road", road},
        {"timestamp_ms", timestamp_ms},
        {"vehicle_count", vehicle_count},
        {"avg_speed_kmh", avg_speed_kmh}
    };
}

TrafficReport TrafficReport::fromJson(const json& j) {
    TrafficReport r;
    r.road = j.at("road").get<std::string>();
    r.timestamp_ms = j.contains("timestamp_ms") ? j.at("timestamp_ms").get<std::int64_t>() : 0;
    r.vehicle_count = j.value("vehicle_count", 0);
    r.avg_speed_kmh = j.value("avg_speed_kmh", 0.0);
    if (!r.valid()) {
        throw std::runtime_error("Invalid TrafficReport");
    }
    return r;
}


