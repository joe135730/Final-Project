# Distributed Traffic Monitoring System

This project simulates a sharded, replicated traffic monitoring system built for CS6650. It demonstrates core distributed systems concepts including hash-based sharding, Raft-lite replication, automatic failover, and real-time data aggregation.

## Components

- **Server (`bin/server`)** – HTTP ingest, shard routing, Raft-lite replication, aggregation.
- **Sensor (`bin/sensor`)** – Simulated traffic sensor clients pushing JSON updates.
- **Config (`config/`)** – Cluster topology and seed road list.
- **Dashboard** – Web-based visualization showing real-time traffic status.

---

## Installation and Compilation on Khoury Linux Cluster

### Prerequisites

1. **SSH Access to Khoury Linux**
   ```bash
   ssh joe135730@linux-075.khoury.northeastern.edu
   ```

2. **Required Software** (should be pre-installed):
   - `g++` compiler with C++11 support
   - `make` build tool
   - Standard C++ libraries

3. **Verify Dependencies**:
   ```bash
   g++ --version    # Should show g++ 11 or later
   make --version   # Should show GNU Make
   ```

### Step-by-Step Installation

#### Step 1: Navigate to Project Directory
```bash
cd ~/Desktop/Final\ Project
# Or wherever your project is located
pwd  # Verify you're in the right directory
```

#### Step 2: Verify Project Structure
```bash
ls -la
# Should show:
# - Makefile
# - config/
# - include/
# - src/
# - third_party/
# - dashboard/
```

#### Step 3: Verify Third-Party Dependencies
```bash
# Check that header-only libraries exist
ls third_party/nlohmann/json.hpp
ls third_party/httplib.h

# If missing, download them:
# wget https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp -O third_party/nlohmann/json.hpp
# wget https://github.com/yhirose/cpp-httplib/releases/download/v0.15.0/httplib.h -O third_party/httplib.h
```

#### Step 4: Clean Previous Builds (Optional)
```bash
make clean
# This removes the bin/ directory if it exists
```

#### Step 5: Compile the Project
```bash
# Build both server and sensor
make

# Or build individually:
make server   # Build only bin/server
make sensor   # Build only bin/sensor
```

**Expected Output:**
```
mkdir -p bin
g++ -Wall -Wextra -O2 -std=c++11 -pthread -Iinclude -Ithird_party src/main_server.cpp src/server/MainServer.cpp ... -o bin/server
g++ -Wall -Wextra -O2 -std=c++11 -pthread -Iinclude -Ithird_party src/main_sensor.cpp src/sensor/SensorRunner.cpp ... -o bin/sensor
```

**If compilation fails:**
- Check error messages for missing dependencies
- Verify all source files exist in `src/` directory
- Ensure `third_party/` contains `json.hpp` and `httplib.h`

#### Step 6: Verify Binaries Were Created
```bash
ls -lh bin/
# Should show:
# -rwxr-xr-x 1 user user ... bin/server
# -rwxr-xr-x 1 user user ... bin/sensor

# Test that binaries are executable
file bin/server
file bin/sensor
```

#### Step 7: Verify Configuration Files
```bash
cat config/cluster.json
cat config/roads.json
# Should show valid JSON
```

---

## Running the System

### Command-Line Syntax

#### Server
```bash
./bin/server <id> <host> <port> <cluster_path> [roads_path] [leader]
```

**Parameters:**
- `<id>`: Server node ID (must match `config/cluster.json`)
- `<host>`: Hostname or IP address (typically `127.0.0.1`)
- `<port>`: HTTP port number (e.g., `5000`, `5001`, `5002`)
- `<cluster_path>`: Path to cluster configuration file (e.g., `config/cluster.json`)
- `[roads_path]`: Optional path to roads list (default: `config/roads.json`)
- `[leader]`: Optional leader flag (`1`, `true`, `yes` to force leader mode)

**Examples:**
```bash
./bin/server 1 127.0.0.1 5000 config/cluster.json
./bin/server 2 127.0.0.1 5001 config/cluster.json config/roads.json
```

#### Sensor
```bash
./bin/sensor <id> <host> <port> [interval_ms] [count]
```

**Parameters:**
- `<id>`: Sensor client ID
- `<host>`: Server hostname or IP address
- `<port>`: Server HTTP port
- `[interval_ms]`: Optional report interval in milliseconds (default: `500`)
- `[count]`: Optional number of concurrent sensor threads (default: `1`)

**Examples:**
```bash
./bin/sensor 1 127.0.0.1 5000
./bin/sensor 1 127.0.0.1 5000 500 5
```

### Running a Complete System

#### Step 1: Start Server 1 (Terminal 1)
```bash
ssh joe135730@linux-075.khoury.northeastern.edu
cd ~/Desktop/Final\ Project
./bin/server 1 127.0.0.1 5000 config/cluster.json
```

**Expected Output:**
```
2025-11-11 17:11:50 [INFO] Starting server 1 on 127.0.0.1:5000
2025-11-11 17:11:50 [INFO] Seeded 8 roads into aggregator
```

#### Step 2: Start Server 2 (Terminal 2)
```bash
ssh joe135730@linux-075.khoury.northeastern.edu
cd ~/Desktop/Final\ Project
./bin/server 2 127.0.0.1 5001 config/cluster.json
```

**Expected Output:**
```
2025-11-11 17:11:51 [INFO] Starting server 2 on 127.0.0.1:5001
2025-11-11 17:11:51 [INFO] Seeded 8 roads into aggregator
```

#### Step 3: Start Server 3 (Terminal 3)
```bash
ssh joe135730@linux-075.khoury.northeastern.edu
cd ~/Desktop/Final\ Project
./bin/server 3 127.0.0.1 5002 config/cluster.json
```

**Expected Output:**
```
2025-11-11 17:11:52 [INFO] Starting server 3 on 127.0.0.1:5002
2025-11-11 17:11:52 [INFO] Seeded 8 roads into aggregator
```

#### Step 4: Start Sensor Client (Terminal 4)
```bash
ssh joe135730@linux-075.khoury.northeastern.edu
cd ~/Desktop/Final\ Project
./bin/sensor 1 127.0.0.1 5000 500 5
```

**Expected Output:**
```
2025-11-11 17:12:00 [INFO] Starting sensor 1 -> 127.0.0.1:5000
```

#### Step 5: Access Dashboard

**Option A: SSH Port Forwarding (Recommended for Windows/Mac)**

On your local machine (Windows PowerShell, Mac Terminal, or Linux):
```bash
ssh -L 5000:127.0.0.1:5000 -L 5001:127.0.0.1:5001 -L 5002:127.0.0.1:5002 joe135730@linux-075.khoury.northeastern.edu
```

Keep this SSH session open, then open in your local browser:
```
http://127.0.0.1:5000/
http://127.0.0.1:5001/
http://127.0.0.1:5002/
```

**Option B: Direct Access (If X11 Forwarding Enabled)**

On the Linux machine:
```bash
firefox http://127.0.0.1:5000/
# Or
google-chrome http://127.0.0.1:5000/
```

**Option C: Network Access (If Firewall Allows)**

If the Linux machine is accessible from your network:
```
http://linux-075.khoury.northeastern.edu:5000/
```

### Stopping the System

Press `Ctrl+C` in each terminal to stop processes gracefully. The system will:
- Stop accepting new connections
- Finish processing current requests
- Clean up resources
- Exit cleanly

---

## Testing the System

### Basic Functionality Tests

#### Test 1: Server Status
```bash
curl http://127.0.0.1:5000/status
```

**Expected:** JSON response with node ID, active shards, and cluster status

#### Test 2: Local Summary
```bash
curl http://127.0.0.1:5000/summary
```

**Expected:** JSON array of road snapshots with traffic metrics

#### Test 3: Specific Road Query
```bash
curl "http://127.0.0.1:5000/data?road=Main_St"
```

**Expected:** JSON object with Main_St traffic data

#### Test 4: Federated Summary
```bash
curl http://127.0.0.1:5000/summary_all
```

**Expected:** JSON array aggregating data from all nodes

### Failover Test

1. Start all 3 servers and 1 sensor client
2. Identify server 1 process:
   ```bash
   ps aux | grep "bin/server 1" | grep -v grep
   ```
3. Kill server 1:
   ```bash
   kill <PID>
   # Or: pkill -f "bin/server 1"
   ```
4. Wait 5-10 seconds
5. Check server 2 or 3 logs - should show promotion messages
6. Verify system continues operating:
   ```bash
   curl http://127.0.0.1:5002/summary_all
   ```

### Multi-Client Load Test

Start multiple sensor clients:
```bash
# Terminal 1
./bin/sensor 1 127.0.0.1 5000 500 3

# Terminal 2
./bin/sensor 2 127.0.0.1 5001 500 3

# Terminal 3
./bin/sensor 3 127.0.0.1 5002 500 3
```

Observe dashboard - should show traffic distributed across all roads.

---

## API Endpoints

- `POST /ingest` - Ingest traffic report from sensor
- `POST /ingest_internal` - Internal replication endpoint
- `GET /summary` - Get local node's aggregated data
- `GET /summary_all` - Get federated view from all nodes
- `GET /data?road=<name>` - Get specific road data
- `GET /status` - Get node health and cluster status
- `GET /sync` - Get full data snapshot for leader promotion
- `GET /` - Dashboard UI (serves `dashboard/index.html`)

---

## Troubleshooting

### Compilation Errors

**Error: "missing terminating ' character"**
- **Cause:** C++17 digit separators (`5'000`) not supported in C++11
- **Fix:** Already fixed in code, but if you see this, check `src/agg/Aggregator.cpp`

**Error: "undefined reference to ..."**
- **Cause:** Missing source files in Makefile
- **Fix:** Verify `Makefile` includes all `.cpp` files in `SRV_SRC` and `SNS_SRC`

**Error: "cannot find -lpthread"**
- **Cause:** Missing pthread library
- **Fix:** Install `build-essential`: `sudo apt-get install build-essential`

### Runtime Errors

**Error: "Address already in use"**
- **Cause:** Port already occupied by another process
- **Fix:** 
  ```bash
  lsof -i :5000  # Find process using port
  kill <PID>     # Kill it
  ```

**Error: "Unable to reach server"**
- **Cause:** Server not running or wrong port
- **Fix:** Verify server is running: `ps aux | grep bin/server`

**Dashboard shows "No Data"**
- **Cause:** No sensor clients running or servers not receiving data
- **Fix:** Start sensor client and wait a few seconds

### Performance Issues

**High CPU Usage**
- **Cause:** Too many sensor threads or short intervals
- **Fix:** Reduce sensor count or increase interval

**Slow Dashboard Updates**
- **Cause:** Network latency or server overload
- **Fix:** Check server logs for errors, reduce sensor load

---

## Project Structure

```
traffic_monitoring/
├── Makefile                 # Build configuration
├── README.md                # This file
├── FINAL_REPORT.md          # Detailed project report
├── config/
│   ├── cluster.json         # Cluster topology (nodes, shards)
│   └── roads.json           # Road list
├── include/                 # Header files
│   ├── agg/
│   ├── net/
│   ├── repl/
│   ├── sensor/
│   ├── server/
│   ├── shard/
│   ├── types/
│   └── util/
├── src/                     # Source files
│   ├── agg/
│   ├── net/
│   ├── repl/
│   ├── sensor/
│   ├── server/
│   ├── shard/
│   ├── types/
│   ├── util/
│   ├── main_server.cpp
│   └── main_sensor.cpp
├── third_party/             # Dependencies
│   ├── nlohmann/
│   │   └── json.hpp
│   └── httplib.h
├── dashboard/               # Dashboard UI
│   ├── index.html
│   └── static/
│       └── map.js
└── bin/                     # Compiled binaries (created after build)
    ├── server
    └── sensor
```

---

## Additional Resources

- **Final Report**: See `FINAL_REPORT.md` for detailed system design, evaluation, and experiment instructions
- **Progress Reports**: See `Progress_Report_1.txt` and `Progress_Report_2.txt` for development history

---

## License

This project is developed for CS6650 - Distributed Systems course at Khoury College of Computer Sciences, Northeastern University.


