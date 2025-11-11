#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "Replication.h"

class RaftLite {
public:
    explicit RaftLite(Replication& parent);
    ~RaftLite();

    void startLeader();
    void startFollower();
    void stop();

    long term() const { return term_.load(); }
    long lastIndex() const { return last_index_.load(); }

    long appendAndBroadcast(const OpLogEntry& entry);
    void applyRemote(const OpLogEntry& entry);

private:
    void heartbeatLoop_();

    Replication& parent_;  // Reserved for future use (e.g., accessing parent config)
    std::atomic<bool> running_{false};
    std::atomic<bool> is_leader_{false};
    std::atomic<long> term_{1};
    std::atomic<long> last_index_{0};

    std::thread heartbeat_thread_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::vector<OpLogEntry> log_;
};


