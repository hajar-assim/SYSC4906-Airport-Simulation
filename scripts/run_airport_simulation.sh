#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make simulator

echo "=== Running T1: Single Lifecycle ==="
./bin/AIRPORT_SIMULATION input_data/T1_single_lifecycle.txt 500
echo "=== Running T2: Burst Test ==="
./bin/AIRPORT_SIMULATION input_data/T2_burst_test.txt 3600
echo "=== Running T3: Staggered Test ==="
./bin/AIRPORT_SIMULATION input_data/T3_staggered_test.txt 18000
echo "=== Running T4: Rapid Test ==="
./bin/AIRPORT_SIMULATION input_data/T4_rapid_test.txt 1800
echo "=== Running T5: Boundary Test ==="
./bin/AIRPORT_SIMULATION input_data/T5_boundary_test.txt 3600
echo "=== Running T6: Bay Stress Test ==="
./bin/AIRPORT_SIMULATION input_data/T6_bay_stress_test.txt 3600
echo "=== All Experiments Complete ==="
