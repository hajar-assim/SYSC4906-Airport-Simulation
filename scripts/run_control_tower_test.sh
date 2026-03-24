#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests
./bin/CONTROL_TOWER_TEST > simulation_results/control_tower_output.txt 2>&1
cat simulation_results/control_tower_output.txt
