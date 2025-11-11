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
make server    # build only the cluster binary
make client    # build only the sensor binary
make           # build both
```

Executables are written to `bin/server` and `bin/sensor`.

## Run Example

1. Start three servers (one per node) in separate terminals:

    ```bash
    ./bin/server --id srv-1 --host 127.0.0.1 --http 5000 --cluster config/cluster.json --leader
    ./bin/server --id srv-2 --host 127.0.0.1 --http 5001 --cluster config/cluster.json
    ./bin/server --id srv-3 --host 127.0.0.1 --http 5002 --cluster config/cluster.json
    ```

2. Launch simulated sensors (e.g. five virtual intersections reporting twice per second):

    ```bash
    ./bin/sensor --id sensorA --server 127.0.0.1:5000 --count 5 --interval 500
    ```

3. Open the realtime dashboard in a browser:

    ```
    http://127.0.0.1:5000/
    ```

    The canvas map paints each road segment (green / yellow / red) and animates vehicle icons based on the most recent 5‑second flow. The metrics panel lists per-road statistics and cluster status.

REST endpoints remain available for scripting/validation (`/summary`, `/data?road=...`, `/status`). Use `Ctrl+C` in each terminal to stop processes gracefully.


