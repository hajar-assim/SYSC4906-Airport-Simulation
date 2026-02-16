CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
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

#TARGET TO COMPILE ALL TESTS TOGETHER
tests: main_control_tower_test.o main_queue_test.o main_runway_test.o main_selector_test.o main_storage_bay_test.o main_merger_test.o plane_message.o
	$(CC) -g -o bin/CONTROL_TOWER_TEST build/main_control_tower_test.o build/plane_message.o
	$(CC) -g -o bin/QUEUE_TEST build/main_queue_test.o build/plane_message.o
	$(CC) -g -o bin/RUNWAY_TEST build/main_runway_test.o build/plane_message.o
	$(CC) -g -o bin/SELECTOR_TEST build/main_selector_test.o build/plane_message.o
	$(CC) -g -o bin/STORAGE_BAY_TEST build/main_storage_bay_test.o build/plane_message.o
	$(CC) -g -o bin/MERGER_TEST build/main_merger_test.o build/plane_message.o

#TARGET TO COMPILE ONLY AIRPORT SIMULATOR
simulator: main_top.o plane_message.o
	$(CC) -g -o bin/AIRPORT_SIMULATION build/main_top.o build/plane_message.o

#TARGET TO COMPILE EVERYTHING (SIMULATOR + TESTS)
all: simulator tests

#CLEAN COMMANDS
clean:
	rm -f bin/* build/*
