#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests

echo "=== Running All Atomic Tests ==="
./bin/CONTROL_TOWER_TEST
./bin/QUEUE_TEST
./bin/RUNWAY_TEST
./bin/SELECTOR_TEST
./bin/STORAGE_BAY_TEST
./bin/MERGER_TEST
echo "=== Running Coupled Tests ==="
./bin/COUPLED_TEST
echo "=== All Tests Complete ==="
