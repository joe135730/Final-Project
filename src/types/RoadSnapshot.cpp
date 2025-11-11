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


