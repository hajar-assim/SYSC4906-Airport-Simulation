CC=g++
CFLAGS=-std=c++17

# Cadmium include path - set CADMIUM_HOME env var or edit this path
CADMIUM_HOME ?= $(HOME)/cadmium_v2
INCLUDECADMIUM=-I $(CADMIUM_HOME)/include
INCLUDEDESTIMES=-I ../../DESTimes/include
INCLUDEJSONEXPORTER=-I ../../CadmiumModelJSONExporter/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE DATA STRUCTURES
plane_message.o: data_structures/plane_message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/plane_message.cpp -o build/plane_message.o

#TARGET TO COMPILE TOP MODEL
main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) $(INCLUDEJSONEXPORTER) top_model/main.cpp -o build/main_top.o

#TARGET TO COMPILE ATOMIC MODEL TESTS
main_control_tower_test.o: test/main_control_tower_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_control_tower_test.cpp -o build/main_control_tower_test.o

main_queue_test.o: test/main_queue_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_queue_test.cpp -o build/main_queue_test.o

main_runway_test.o: test/main_runway_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_runway_test.cpp -o build/main_runway_test.o

main_selector_test.o: test/main_selector_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_selector_test.cpp -o build/main_selector_test.o

main_storage_bay_test.o: test/main_storage_bay_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_storage_bay_test.cpp -o build/main_storage_bay_test.o

main_merger_test.o: test/main_merger_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_merger_test.cpp -o build/main_merger_test.o

main_coupled_test.o: test/main_coupled_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_coupled_test.cpp -o build/main_coupled_test.o

#TARGET TO COMPILE ALL TESTS TOGETHER
tests: main_control_tower_test.o main_queue_test.o main_runway_test.o main_selector_test.o main_storage_bay_test.o main_merger_test.o main_coupled_test.o plane_message.o
	$(CC) -g -o bin/CONTROL_TOWER_TEST build/main_control_tower_test.o build/plane_message.o
	$(CC) -g -o bin/QUEUE_TEST build/main_queue_test.o build/plane_message.o
	$(CC) -g -o bin/RUNWAY_TEST build/main_runway_test.o build/plane_message.o
	$(CC) -g -o bin/SELECTOR_TEST build/main_selector_test.o build/plane_message.o
	$(CC) -g -o bin/STORAGE_BAY_TEST build/main_storage_bay_test.o build/plane_message.o
	$(CC) -g -o bin/MERGER_TEST build/main_merger_test.o build/plane_message.o
	$(CC) -g -o bin/COUPLED_TEST build/main_coupled_test.o build/plane_message.o

#TARGET TO COMPILE ONLY AIRPORT SIMULATOR
simulator: main_top.o plane_message.o
	$(CC) -g -o bin/AIRPORT_SIMULATION build/main_top.o build/plane_message.o

#TARGET TO COMPILE EVERYTHING (SIMULATOR + TESTS)
all: simulator tests

#RUN EXPERIMENTS
runexperiments: simulator
	@echo "=== Running T1: Single Lifecycle ==="
	./bin/AIRPORT_SIMULATION input_data/T1_single_lifecycle.txt 500
	@echo "=== Running T2: Burst Test ==="
	./bin/AIRPORT_SIMULATION input_data/T2_burst_test.txt 3600
	@echo "=== Running T3: Staggered Test ==="
	./bin/AIRPORT_SIMULATION input_data/T3_staggered_test.txt 18000
	@echo "=== Running T4: Rapid Test ==="
	./bin/AIRPORT_SIMULATION input_data/T4_rapid_test.txt 1800
	@echo "=== Running T5: Boundary Test ==="
	./bin/AIRPORT_SIMULATION input_data/T5_boundary_test.txt 3600
	@echo "=== Running T6: Bay Stress Test ==="
	./bin/AIRPORT_SIMULATION input_data/T6_bay_stress_test.txt 3600
	@echo "=== All Experiments Complete ==="

run_T1: simulator
	./bin/AIRPORT_SIMULATION input_data/T1_single_lifecycle.txt 500

run_T2: simulator
	./bin/AIRPORT_SIMULATION input_data/T2_burst_test.txt 3600

run_T3: simulator
	./bin/AIRPORT_SIMULATION input_data/T3_staggered_test.txt 18000

run_T4: simulator
	./bin/AIRPORT_SIMULATION input_data/T4_rapid_test.txt 1800

run_T5: simulator
	./bin/AIRPORT_SIMULATION input_data/T5_boundary_test.txt 3600

run_T6: simulator
	./bin/AIRPORT_SIMULATION input_data/T6_bay_stress_test.txt 3600

#RUN ALL TESTS
runalltests: tests
	@echo "=== Running All Atomic Tests ==="
	./bin/CONTROL_TOWER_TEST
	./bin/QUEUE_TEST
	./bin/RUNWAY_TEST
	./bin/SELECTOR_TEST
	./bin/STORAGE_BAY_TEST
	./bin/MERGER_TEST
	@echo "=== Running Coupled Tests ==="
	./bin/COUPLED_TEST
	@echo "=== All Tests Complete ==="

#CLEAN COMMANDS
clean:
	rm -f bin/* build/*

.PHONY: all simulator tests clean runexperiments runalltests run_T1 run_T2 run_T3 run_T4 run_T5 run_T6
