#include "repl/RaftLite.h"

#include <chrono>
#include <thread>

#include "util/Logger.h"

RaftLite::RaftLite(Replication& parent)
    : parent_(parent) {
    (void)parent_;  // Suppress unused warning - reserved for future use
}

RaftLite::~RaftLite() {
    stop();
}

void RaftLite::startLeader() {
    is_leader_.store(true);
    running_.store(true);
    heartbeat_thread_ = std::thread([this]() { heartbeatLoop_(); });
}

void RaftLite::startFollower() {
    is_leader_.store(false);
    running_.store(true);
    heartbeat_thread_ = std::thread([this]() { heartbeatLoop_(); });
}

void RaftLite::stop() {
    running_.store(false);
    cv_.notify_all();
    if (heartbeat_thread_.joinable()) {
        heartbeat_thread_.join();
    }
}

long RaftLite::appendAndBroadcast(const OpLogEntry& entry) {
    std::lock_guard<std::mutex> g(mtx_);
    OpLogEntry copy = entry;
    copy.index = last_index_.load() + 1;
    copy.term = term_.load();
    log_.push_back(copy);
    last_index_.store(copy.index);
    return copy.index;
}

void RaftLite::applyRemote(const OpLogEntry& entry) {
    std::lock_guard<std::mutex> g(mtx_);
    log_.push_back(entry);
    last_index_.store(entry.index);
    if (entry.term > term_) {
        term_.store(entry.term);
    }
}

void RaftLite::heartbeatLoop_() {
    while (running_.load()) {
        std::unique_lock<std::mutex> lk(mtx_);
        cv_.wait_for(lk, std::chrono::seconds(2));
        if (!running_.load()) break;
        if (is_leader_.load()) {
            Logger::instance().debug("Heartbeat term=" + std::to_string(term_.load()));
        }
    }
}


