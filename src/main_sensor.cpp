#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <map>
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

std::map<std::string, std::string> parseArgs(int argc, char** argv) {
    std::map<std::string, std::string> out;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind("--", 0) == 0) {
            std::string key = arg.substr(2);
            std::string value = "true";
            if (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0) {
                value = argv[++i];
            }
            out[key] = value;
        }
    }
    return out;
}
}

int main(int argc, char** argv) {
    auto args = parseArgs(argc, argv);

    if (!args.count("id") || !args.count("server")) {
        std::cerr << "Usage: sensor --id S1 --server host:port [--roads config/roads.json] "
                     "[--interval 500] [--count 5]\n";
        return 1;
    }

    std::string id = args["id"];
    std::string server = args["server"];
    auto colon = server.find(':');
    if (colon == std::string::npos) {
        std::cerr << "--server must be host:port\n";
        return 1;
    }
    std::string host = server.substr(0, colon);
    int port = std::stoi(server.substr(colon + 1));
    std::string roadsPath = args.count("roads") ? args["roads"] : "config/roads.json";
    int interval = args.count("interval") ? std::stoi(args["interval"]) : 500;
    int count = args.count("count") ? std::stoi(args["count"]) : 1;

    try {
        auto roads = JsonIO::loadRoadList(roadsPath);
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


