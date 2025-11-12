#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "sensor/SensorRunner.h"
#include "util/JsonIO.h"
#include "util/Logger.h"

namespace {
std::atomic<bool> keep_running{true};

void signalHandler(int) {
    keep_running.store(false);
}
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: sensor <id> <host> <port> [interval_ms] [count]\n";
        return 1;
    }

    std::string id = argv[1];
    std::string host = argv[2];
    int port = std::stoi(argv[3]);

    int interval = 500;
    int count = 1;

    if (argc >= 5) {
        interval = std::stoi(argv[4]);
    }
    if (argc >= 6) {
        count = std::stoi(argv[5]);
    }

    try {
        auto roads = JsonIO::loadRoadList("config/roads.json");
        if (roads.empty()) {
            throw std::runtime_error("Road list is empty");
        }
        std::signal(SIGINT, signalHandler);
#ifdef SIGTERM
        std::signal(SIGTERM, signalHandler);
#endif
        SensorRunner runner(id, host, port, roads, interval);
        Logger::instance().info("Starting sensor " + id + " -> " + host + ":" + std::to_string(port));
        runner.start(count);
        while (keep_running.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        Logger::instance().info("Stopping sensors");
        runner.stop();
    } catch (const std::exception& ex) {
        Logger::instance().error(std::string("Sensor failed: ") + ex.what());
        return 1;
    }
    return 0;
}


