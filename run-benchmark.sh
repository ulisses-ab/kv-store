#!/bin/bash
make

BENCH_CMD="redis-benchmark -p 5555 -n 40000 -c 100 -t set"

./program.out &
SERVER_PID=$!

echo "Server PID: $SERVER_PID"

sleep 0.1

echo "Running benchmark..."
eval "$BENCH_CMD"

echo "Benchmark finished. Killing server..."
kill $SERVER_PID