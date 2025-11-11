#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include <httplib.h>

class HttpServer {
public:
    using Handler = std::function<void(const httplib::Request&, httplib::Response&)>;

    HttpServer();
    ~HttpServer();

    void Get(const std::string& path, Handler handler);
    void Post(const std::string& path, Handler handler);

    bool start(const std::string& host, int port);
    void stop();

private:
    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;
    std::atomic<bool> running_{false};
};


