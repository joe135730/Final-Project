#include "agg/Aggregator.h"

#include <algorithm>
#include <chrono>

#include "util/Time.h"

namespace {
const long WINDOW5_MS = 5000;
const long WINDOW60_MS = 60000;
}

void Aggregator::add(const TrafficReport& r) {
    std::lock_guard<std::mutex> g(mtx_);
    auto& win = win_[r.road];
    win.w5s.push_back(r);
    win.w60s.push_back(r);
    win.last_ts = r.timestamp_ms;
    win.ewma = win.ewma == 0 ? r.avg_speed_kmh : 0.7 * win.ewma + 0.3 * r.avg_speed_kmh;
}

void Aggregator::recompute(long now_ms) {
    std::lock_guard<std::mutex> g(mtx_);
    for (auto& kv : win_) {
        const std::string& road = kv.first;
        Window& window = kv.second;
        while (!window.w5s.empty() && now_ms - window.w5s.front().timestamp_ms > WINDOW5_MS) {
            window.w5s.pop_front();
        }
        while (!window.w60s.empty() && now_ms - window.w60s.front().timestamp_ms > WINDOW60_MS) {
            window.w60s.pop_front();
        }

        // Only update snapshot if we have data in windows, otherwise preserve last known good snapshot
        bool hasData = !window.w5s.empty() || !window.w60s.empty();
        if (hasData) {
            // We have data, calculate and update snapshot
            RoadSnapshot snap;
            snap.road = road;
            snap.last_report_ms = window.last_ts;
            snap.cars_5s = 0;
            snap.cars_60s = 0;
            double sum_speed = 0.0;
            for (const auto& r : window.w5s) {
                snap.cars_5s += r.vehicle_count;
            }
            for (const auto& r : window.w60s) {
                snap.cars_60s += r.vehicle_count;
                sum_speed += r.avg_speed_kmh;
            }
            snap.avg_speed_kmh = window.w60s.empty() ? 0.0 : sum_speed / window.w60s.size();
            snap.ewma_speed_kmh = window.ewma;
            snap.classification = classify(snap.avg_speed_kmh, snap.cars_5s);
            last_[road] = snap;
        } else if (last_.find(road) != last_.end()) {
            // No data in windows, but we have a previous snapshot - preserve it
            auto& existing = last_[road];
            long stale_threshold = 120000; // 2 minutes
            if (now_ms - existing.last_report_ms > stale_threshold) {
                // Mark as stale by setting classification to indicate no recent data
                existing.classification = "STALE";
            }
            // Keep the existing values (cars_5s, cars_60s, etc.) - don't update
        }
        // If no data and no previous snapshot, do nothing (road will not appear in summary)
    }
}

bool Aggregator::get(const std::string& road, RoadSnapshot& out) const {
    std::lock_guard<std::mutex> g(mtx_);
    auto it = last_.find(road);
    if (it == last_.end()) return false;
    out = it->second;
    return true;
}

std::vector<RoadSnapshot> Aggregator::summary(size_t topN) const {
    std::lock_guard<std::mutex> g(mtx_);
    std::vector<RoadSnapshot> v;
    v.reserve(last_.size());
    for (const auto& kv : last_) {
        v.push_back(kv.second);
    }
    std::sort(v.begin(), v.end(), [](const RoadSnapshot& a, const RoadSnapshot& b) {
        if (a.classification == b.classification) {
            return a.cars_60s > b.cars_60s;
        }
        auto rank = [](const std::string& cls) {
            if (cls == "CONGESTED") return 0;
            if (cls == "MODERATE") return 1;
            return 2;
        };
        return rank(a.classification) < rank(b.classification);
    });
    if (v.size() > topN) {
        v.resize(topN);
    }
    return v;
}

std::string Aggregator::classify(double speed_kmh, int cars5s) {
    if (cars5s > 40 || speed_kmh < 15.0) return "CONGESTED";
    if (cars5s > 20 || speed_kmh < 35.0) return "MODERATE";
    return "SMOOTH";
}


