#include "agg/Aggregator.h"
#include "util/Time.h"

#include <cassert>

int main() {
    Aggregator agg;
    long now = nowMs();
    TrafficReport r;
    r.road = "Main_St";
    r.timestamp_ms = now;
    r.vehicle_count = 30;
    r.avg_speed_kmh = 20.0;
    agg.add(r);
    agg.recompute(now);
    RoadSnapshot snap;
    bool ok = agg.get("Main_St", snap);
    assert(ok);
    assert(snap.cars_5s == 30);
    return 0;
}


