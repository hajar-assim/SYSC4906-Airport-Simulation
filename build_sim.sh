#!/bin/bash

# Check if CADMIUM environment variable is set
if [ -z "$CADMIUM" ]; then
    echo "ERROR: CADMIUM environment variable is not set!"
    echo "Please set it to your Cadmium v2 include directory:"
    echo "  export CADMIUM=/path/to/cadmium_v2/include"
    echo ""
    echo "Example:"
    echo "  export CADMIUM=/Users/username/cadmium_v2/include"
    exit 1
fi

if [ -d "build" ]; then rm -Rf build; fi
if [ -d "bin" ]; then rm -Rf bin; fi
mkdir -p build
mkdir -p bin
mkdir -p simulation_results
make clean
make all
echo "Compilation done. Executables in the bin folder"
