#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"$ROOT/bin/server" --id srv-1 --host 127.0.0.1 --http 5000 --cluster "$ROOT/config/cluster.json" --leader &
PID1=$!
"$ROOT/bin/server" --id srv-2 --host 127.0.0.1 --http 5001 --cluster "$ROOT/config/cluster.json" &
PID2=$!
"$ROOT/bin/server" --id srv-3 --host 127.0.0.1 --http 5002 --cluster "$ROOT/config/cluster.json" &
PID3=$!

trap "kill $PID1 $PID2 $PID3" INT TERM

wait


