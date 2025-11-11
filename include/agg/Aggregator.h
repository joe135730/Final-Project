#pragma once
#include <string>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <vector>
#include "types/TrafficReport.h"
#include "types/RoadSnapshot.h"

class Aggregator {
public:
    void add(const TrafficReport& r);    // called on commit/apply
    void recompute(long now_ms);         // 1 Hz
    bool get(const std::string& road, RoadSnapshot& out) const;
    std::vector<RoadSnapshot> summary(size_t topN=20) const;
private:
    struct Window { std::deque<TrafficReport> w5s, w60s; double ewma=0; long last_ts=0; };
    mutable std::mutex mtx_;
    std::unordered_map<std::string, Window> win_;
    std::unordered_map<std::string, RoadSnapshot> last_;
    static std::string classify(double speed_kmh, int cars5s);
};
