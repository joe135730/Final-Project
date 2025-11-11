#include "net/HttpClient.h"

#include <httplib.h>

HttpClient::HttpClient(std::string host, int port)
    : host_(std::move(host)), port_(port) {}

bool HttpClient::postJson(const std::string& path,
                          const nlohmann::json& body,
                          nlohmann::json* response,
                          int timeout_ms) {
    httplib::Client cli(host_, port_);
    cli.set_connection_timeout(timeout_ms / 1000, (timeout_ms % 1000) * 1000);
    auto res = cli.Post(path.c_str(), body.dump(), "application/json");
    if (!res || res->status >= 400) {
        return false;
    }
    if (response) {
        if (res->body.empty()) {
            *response = nlohmann::json::object();
        } else {
            *response = nlohmann::json::parse(res->body);
        }
    }
    return true;
}

bool HttpClient::getJson(const std::string& path, nlohmann::json* response, int timeout_ms) {
    httplib::Client cli(host_, port_);
    cli.set_connection_timeout(timeout_ms / 1000, (timeout_ms % 1000) * 1000);
    auto res = cli.Get(path.c_str());
    if (!res || res->status >= 400) {
        return false;
    }
    if (response) {
        if (res->body.empty()) {
            *response = nlohmann::json::object();
        } else {
            *response = nlohmann::json::parse(res->body);
        }
    }
    return true;
}


