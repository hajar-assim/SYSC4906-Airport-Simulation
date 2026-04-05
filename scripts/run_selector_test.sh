#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

make clean && make tests
./bin/SELECTOR_TEST > simulation_results/selector_output.txt 2>&1
cat simulation_results/selector_output.txt
