#include "types/RoadSnapshot.h"

using nlohmann::json;

json RoadSnapshot::toJson() const {
    return json{
        {"road", road},
        {"last_report_ms", last_report_ms},
        {"cars_5s", cars_5s},
        {"cars_60s", cars_60s},
        {"avg_speed_kmh", avg_speed_kmh},
        {"ewma_speed_kmh", ewma_speed_kmh},
        {"classification", classification}
    };
}

RoadSnapshot RoadSnapshot::fromJson(const json& j) {
    RoadSnapshot snap;
    snap.road = j.at("road").get<std::string>();
    snap.last_report_ms = j.at("last_report_ms").get<std::int64_t>();
    snap.cars_5s = j.at("cars_5s").get<int>();
    snap.cars_60s = j.at("cars_60s").get<int>();
    snap.avg_speed_kmh = j.at("avg_speed_kmh").get<double>();
    snap.ewma_speed_kmh = j.at("ewma_speed_kmh").get<double>();
    snap.classification = j.at("classification").get<std::string>();
    return snap;
}


