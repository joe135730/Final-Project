# Distributed Traffic Monitoring

This project simulates a sharded, replicated traffic monitoring system built for CS6650.

## Components

- **Server (`bin/server`)** – HTTP ingest, shard routing, Raft-lite replication, aggregation.
- **Sensor (`bin/sensor`)** – Simulated traffic sensor clients pushing JSON updates.
- **Config (`config/`)** – Cluster topology and seed road list.

## Prerequisites

1. C++11 toolchain (`g++`).
2. Header-only dependencies located under `third_party/`:
   - [`json.hpp`](https://github.com/nlohmann/json/releases)
   - [`httplib.h`](https://github.com/yhirose/cpp-httplib/releases)

## Build

```bash
make -j
```

Executables are written to `bin/server` and `bin/sensor`.

## Run Example

Terminal 1:

```bash
./bin/server --id srv-1 --host 127.0.0.1 --http 5000 --cluster config/cluster.json --leader
```

Terminal 2:

```bash
./bin/server --id srv-2 --host 127.0.0.1 --http 5001 --cluster config/cluster.json
```

Terminal 3:

```bash
./bin/server --id srv-3 --host 127.0.0.1 --http 5002 --cluster config/cluster.json
```

Terminal 4 (sensor load generator):

```bash
./bin/sensor --id sensorA --server 127.0.0.1:5000 --count 5 --interval 500
```

Query summaries:

```bash
curl http://127.0.0.1:5000/summary | jq
curl "http://127.0.0.1:5000/data?road=Main_St" | jq
curl http://127.0.0.1:5000/status | jq
```

Use `Ctrl+C` in each terminal to stop processes gracefully.


