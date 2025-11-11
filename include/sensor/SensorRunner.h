#pragma once

#include <atomic>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "net/HttpClient.h"
#include "types/TrafficReport.h"

class SensorRunner {
public:
    SensorRunner(std::string id,
                 std::string server_host,
                 int server_port,
                 std::vector<std::string> roads,
                 int interval_ms = 500);
    ~SensorRunner();

    void start(int sensors = 1);
    void stop();

private:
    void loop_(int idx);
    TrafficReport makeReport_(const std::string& road);

    std::string id_;
    std::string server_host_;
    int server_port_{0};
    std::vector<std::string> roads_;
    int interval_ms_{500};

    std::atomic<bool> running_{false};
    std::vector<std::thread> workers_;
    std::mt19937 rng_;
    HttpClient client_;
};


