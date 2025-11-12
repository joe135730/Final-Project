#include "sensor/SensorRunner.h"

#include <chrono>
#include <thread>

#include "util/Time.h"

SensorRunner::SensorRunner(std::string id,
                           std::string server_host,
                           int server_port,
                           std::vector<std::string> roads,
                           int interval_ms)
    : id_(std::move(id)),
      server_host_(std::move(server_host)),
      server_port_(server_port),
      roads_(std::move(roads)),
      interval_ms_(interval_ms),
      rng_(std::random_device{}()),
      client_(server_host_, server_port_) {
    // Set up failover servers (common ports for local testing)
    // Try primary first, then 5001, 5002, 5003
    failover_servers_.push_back({server_host_, server_port_});
    if (server_port_ != 5001) failover_servers_.push_back({server_host_, 5001});
    if (server_port_ != 5002) failover_servers_.push_back({server_host_, 5002});
    if (server_port_ != 5003) failover_servers_.push_back({server_host_, 5003});
}

SensorRunner::~SensorRunner() {
    stop();
}

void SensorRunner::start(int sensors) {
    if (running_.exchange(true)) return;
    workers_.reserve(sensors);
    for (int i = 0; i < sensors; ++i) {
        workers_.emplace_back([this, i]() { loop_(i); });
    }
}

void SensorRunner::stop() {
    if (!running_.exchange(false)) return;
    for (auto& t : workers_) {
        if (t.joinable()) {
            t.join();
        }
    }
    workers_.clear();
}

void SensorRunner::loop_(int idx) {
    std::uniform_int_distribution<std::size_t> road_dist(0, roads_.size() - 1);
    std::uniform_int_distribution<int> cars_dist(0, 50);
    std::uniform_real_distribution<double> speed_dist(5.0, 70.0);
    while (running_.load()) {
        auto road = roads_[road_dist(rng_)];
        auto report = makeReport_(road);
        report.vehicle_count = cars_dist(rng_);
        report.avg_speed_kmh = speed_dist(rng_);
        auto body = report.toJson();
        body["sensor_id"] = id_ + "-" + std::to_string(idx);
        
        // Try sending with failover
        if (!trySendWithFailover(body)) {
            // All servers failed, log but continue
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
    }
}

bool SensorRunner::trySendWithFailover(const nlohmann::json& body) {
    for (const auto& server : failover_servers_) {
        HttpClient cli(server.first, server.second);
        if (cli.postJson("/ingest", body, nullptr, 500)) {
            return true;  // Success
        }
    }
    return false;  // All servers failed
}

TrafficReport SensorRunner::makeReport_(const std::string& road) {
    TrafficReport r;
    r.road = road;
    r.timestamp_ms = nowMs();
    r.vehicle_count = 0;
    r.avg_speed_kmh = 0.0;
    return r;
}


