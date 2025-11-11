#include "shard/ShardRouter.h"

#include <cassert>

int main() {
    ClusterConfig cfg;
    cfg.partitions = 6;
    ShardInfo shard0;
    shard0.id = 0;
    shard0.leader = "srv-1";
    cfg.shards[0] = shard0;
    Node node;
    node.id = "srv-1";
    node.host = "127.0.0.1";
    node.http_port = 5000;
    cfg.nodes[node.id] = node;

    ShardRouter router(cfg);
    int shardId = router.shardFor("Main_St");
    assert(shardId >= 0 && shardId < cfg.partitions);
    const Node& leader = router.leaderFor(0);
    assert(leader.id == "srv-1");
    return 0;
}


