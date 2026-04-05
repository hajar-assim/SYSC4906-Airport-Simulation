#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests

echo "=== Running All Atomic Tests ==="
echo "Running Control Tower Test..."
./bin/CONTROL_TOWER_TEST > simulation_results/control_tower_output.txt 2>&1
cat simulation_results/control_tower_output.txt

echo "Running Queue Test..."
./bin/QUEUE_TEST > simulation_results/queue_output.txt 2>&1
cat simulation_results/queue_output.txt

echo "Running Runway Test..."
./bin/RUNWAY_TEST > simulation_results/runway_output.txt 2>&1
cat simulation_results/runway_output.txt

echo "Running Selector Test..."
./bin/SELECTOR_TEST > simulation_results/selector_output.txt 2>&1
cat simulation_results/selector_output.txt

echo "Running Storage Bay Test..."
./bin/STORAGE_BAY_TEST > simulation_results/storage_bay_output.txt 2>&1
cat simulation_results/storage_bay_output.txt

echo "Running Merger Test..."
./bin/MERGER_TEST > simulation_results/merger_output.txt 2>&1
cat simulation_results/merger_output.txt

echo "=== Running Coupled Tests ==="
./bin/COUPLED_TEST > simulation_results/coupled_output.txt 2>&1
cat simulation_results/coupled_output.txt

echo "=== All Tests Complete ==="
