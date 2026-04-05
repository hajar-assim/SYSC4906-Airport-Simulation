#!/bin/bash
if [ -d "build" ]; then rm -Rf build; fi
if [ -d "bin" ]; then rm -Rf bin; fi
mkdir -p build
mkdir -p bin
mkdir -p simulation_results
make clean
make all
echo "Compilation done. Executables in the bin folder"
