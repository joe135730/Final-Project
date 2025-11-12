#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <algorithm>
#include <string>
#include <thread>

#include "server/MainServer.h"
#include "util/JsonIO.h"
#include "util/Logger.h"

namespace {
std::atomic<bool> keep_running{true};

void signalHandler(int) {
    keep_running.store(false);
}

}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: server <id> <host> <port> <cluster_path> [roads_path] [leader]" << std::endl;
        return 1;
    }

    std::string id = argv[1];
    std::string host = argv[2];
    int http_port = std::stoi(argv[3]);
    std::string cluster_path = argv[4];
    std::string roads_path = "config/roads.json";
    bool leader = false;

    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    if (argc >= 6) {
        std::string arg5Lower = toLower(argv[5]);
        if (arg5Lower == "1" || arg5Lower == "true" || arg5Lower == "yes" ||
            arg5Lower == "0" || arg5Lower == "false" || arg5Lower == "no") {
            leader = (arg5Lower == "1" || arg5Lower == "true" || arg5Lower == "yes");
        } else {
            roads_path = argv[5];
            if (argc >= 7) {
                std::string leaderArg = toLower(argv[6]);
                leader = (leaderArg == "1" || leaderArg == "true" || leaderArg == "yes");
            }
        }
    }

    try {
        auto cfg = JsonIO::loadClusterConfig(cluster_path);
        MainServer server(id, cfg, host, http_port, leader, roads_path);

        std::signal(SIGINT, signalHandler);
#ifdef SIGTERM
        std::signal(SIGTERM, signalHandler);
#endif

        Logger::instance().info("Starting server " + id + " on " + host + ":" + std::to_string(http_port));
        server.start();
        while (keep_running.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        Logger::instance().info("Stopping server " + id);
        server.stop();
    } catch (const std::exception& ex) {
        Logger::instance().error(std::string("Fatal: ") + ex.what());
        return 1;
    }
    return 0;
}


