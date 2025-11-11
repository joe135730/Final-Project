#pragma once
#include <string>
#include "types/ShardTypes.h"

class ShardRouter {
public:
    explicit ShardRouter(const ClusterConfig& cfg);
    int shardFor(const std::string& road) const;          // hash(road)%P
    const Node& leaderFor(int shardId) const;
    const Node& followerFor(int shardId) const;           // first follower
private:
    ClusterConfig cfg_;
};
