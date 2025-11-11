#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <map>
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
    if (!args.count("id")) {
        std::cerr << "Usage: server --id <node-id> [--host 127.0.0.1] --http <port> "
                     "[--cluster config/cluster.json] [--leader]\n";
        return 1;
    }

    std::string id = args["id"];
    std::string host = args.count("host") ? args["host"] : "127.0.0.1";
    int http_port = args.count("http") ? std::stoi(args["http"]) : 5000;
    std::string cluster_path = args.count("cluster") ? args["cluster"] : "config/cluster.json";
    bool leader = args.count("leader") ? (args["leader"] != "false") : false;

    try {
        auto cfg = JsonIO::loadClusterConfig(cluster_path);
        MainServer server(id, cfg, host, http_port, leader);

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


