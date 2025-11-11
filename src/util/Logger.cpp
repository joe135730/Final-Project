#include "util/Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

#include "util/Time.h"

namespace {
std::string levelToString(LogLevel lvl) {
    switch (lvl) {
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kInfo: return "INFO";
    case LogLevel::kWarn: return "WARN";
    case LogLevel::kError: return "ERROR";
    }
    return "INFO";
}
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::setLevel(LogLevel lvl) {
    std::lock_guard<std::mutex> g(mtx_);
    level_ = lvl;
}

LogLevel Logger::level() const {
    std::lock_guard<std::mutex> g(mtx_);
    return level_;
}

void Logger::debug(const std::string& msg) { log(LogLevel::kDebug, msg); }
void Logger::info(const std::string& msg) { log(LogLevel::kInfo, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::kWarn, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::kError, msg); }

void Logger::log(LogLevel lvl, const std::string& msg) {
    std::lock_guard<std::mutex> g(mtx_);
    if (lvl < level_) return;
    std::ostringstream oss;
    oss << formatTimestamp(nowMs()) << " [" << levelToString(lvl) << "] " << msg;
    std::cout << oss.str() << std::endl;
}

std::string formatLogPrefix(LogLevel lvl) {
    return "[" + levelToString(lvl) + "]";
}


