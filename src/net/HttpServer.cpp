#include "net/HttpServer.h"

#include <atomic>
#include <functional>
#include <stdexcept>

HttpServer::HttpServer()
    : server_(std::unique_ptr<httplib::Server>(new httplib::Server())) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::Get(const std::string& path, Handler handler) {
    server_->Get(path.c_str(), handler);
}

void HttpServer::Post(const std::string& path, Handler handler) {
    server_->Post(path.c_str(), handler);
}

bool HttpServer::start(const std::string& host, int port) {
    if (running_.exchange(true)) {
        return false;
    }
    server_thread_ = std::thread([this, host, port]() {
        server_->listen(host.c_str(), port);
    });
    return true;
}

void HttpServer::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    if (server_) {
        server_->stop();
    }
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}


