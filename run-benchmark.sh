#!/bin/bash

BENCH_CMD="redis-benchmark -p 5555 -n 400000 -c 50 SET key value"

perf record -g ./program.out &
SERVER_PID=$!

echo "Server PID: $SERVER_PID"

sleep 0.1

echo "Running benchmark..."
eval "$BENCH_CMD"

echo "Benchmark finished. Killing server..."
kill $SERVER_PID