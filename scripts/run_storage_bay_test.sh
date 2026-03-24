#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests
./bin/STORAGE_BAY_TEST > simulation_results/storage_bay_output.txt 2>&1
cat simulation_results/storage_bay_output.txt
