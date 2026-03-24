#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests
./bin/MERGER_TEST > simulation_results/merger_output.txt 2>&1
cat simulation_results/merger_output.txt
