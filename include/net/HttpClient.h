#pragma once

#include <string>
#include <nlohmann/json.hpp>

class HttpClient {
public:
    HttpClient(std::string host, int port);

    bool postJson(const std::string& path, const nlohmann::json& body,
                  nlohmann::json* response = nullptr, int timeout_ms = 1000);
    bool getJson(const std::string& path, nlohmann::json* response = nullptr, int timeout_ms = 1000);

private:
    std::string host_;
    int port_;
};


