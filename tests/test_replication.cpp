#include "repl/Replication.h"

#include <cassert>

int main() {
    ClusterConfig cfg;
    cfg.partitions = 1;
    cfg.replication_factor = 1;
    Node node{"srv-1", "127.0.0.1", 5000};
    cfg.nodes[node.id] = node;
    ShardInfo shard;
    shard.id = 0;
    shard.leader = "srv-1";
    cfg.shards[0] = shard;

    Replication repl("srv-1", cfg);
    int applied = 0;
    repl.startLeader([&](const OpLogEntry& entry) {
        (void)entry;
        ++applied;
    });

    TrafficReport r;
    r.road = "Main_St";
    r.timestamp_ms = 0;
    r.vehicle_count = 10;
    r.avg_speed_kmh = 25.0;

    OpLogEntry entry;
    entry.payload = r;
    entry.shard_id = 0;
    repl.appendAndReplicate(entry);
    repl.stop();
    assert(applied == 1);
    return 0;
}


