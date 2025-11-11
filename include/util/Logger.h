#pragma once

#include <mutex>
#include <string>

enum class LogLevel {
    kDebug = 0,
    kInfo,
    kWarn,
    kError
};

class Logger {
public:
    static Logger& instance();
    void setLevel(LogLevel lvl);
    LogLevel level() const;

    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);

private:
    Logger() = default;
    void log(LogLevel lvl, const std::string& msg);

    mutable std::mutex mtx_;
    LogLevel level_{LogLevel::kInfo};
};

std::string formatLogPrefix(LogLevel lvl);


