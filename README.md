AIRPORT SIMULATION DEVS MODEL
This folder contains the Airport Simulation DEVS model implemented in Cadmium.

REPO: https://github.com/hasibkhodayar/SYSC4906-Airport-Simulation

AUTHORS: Hasib Khodayar & Hajar Assim
COURSE: SYSC 4906G, Winter 2026, Carleton University

FILES ORGANIZATION

- README.md         [This file]
- makefile          [Compilation instructions for tests and top model]

atomics/ [This folder contains atomic model headers implemented in Cadmium]
- controlTower.hpp       [Coordinates runway access between landing and takeoff]
- queue.hpp              [FIFO buffer with stop/done flow control]
- runway.hpp             [Processes aircraft landing/takeoff operations]
- selector.hpp           [Routes planes to storage bays based on ID]
- storageBay.hpp         [Stores planes, drains to merger]
- merger.hpp             [Combines outputs from 4 bays into single stream]

coupled/ [This folder contains coupled model headers]
- hangar.hpp             [Top-level hangar: Selector + StorageBank]
- storageBank.hpp        [Internal coupled model: 4 Bays + Merger]

data_structures/
- plane_message.hpp
- plane_message.cpp

- bin/ [This folder will be created automatically during compilation. It contains executables]

- build/ [This folder will be created automatically during compilation. It contains .o build files]

input_data/ [This folder contains all the input event trajectories for simulation]
- control_tower/         [CT-1 to CT-4: ControlTower atomic tests]
- queue/                 [Q-1 to Q-5: Queue atomic tests]
- runway/                [R-1 to R-3: Runway atomic tests]
- selector/              [S-1 to S-5: Selector atomic tests]
- storage_bay/           [SB-1 to SB-2: StorageBay atomic tests]
- merger/                [M-1 to M-3: Merger atomic tests]
- storage_bank/          [SBK-1 to SBK-2: StorageBank coupled tests]
- hangar/                [H-1 to H-2: Hangar coupled tests]
- T1-T6_*.txt            [Top model experiment input files]

scripts/ [This folder contains shell scripts to compile and run each test/simulation]
- run_all_tests.sh               [Compiles and runs all atomic + coupled tests]
- run_control_tower_test.sh
- run_queue_test.sh
- run_runway_test.sh
- run_selector_test.sh
- run_storage_bay_test.sh
- run_merger_test.sh
- run_coupled_test.sh
- run_airport_simulation.sh      [Compiles and runs all T1-T6 experiments]

test/ [This folder contains test driver files for atomic and coupled models]
- main_control_tower_test.cpp
- main_queue_test.cpp
- main_runway_test.cpp
- main_selector_test.cpp
- main_storage_bay_test.cpp
- main_merger_test.cpp
- main_coupled_test.cpp    [StorageBank and Hangar coupled tests]

top_model/ [This folder contains the integrated Airport Simulation driver]
- main.cpp

simulation_results/ [This folder stores the execution logs from simulations]
- Output CSV files are generated here automatically when experiments are run.

EXECUTION STEPS

1 - Update the include path in the project 'makefile'. Ensure the following line points to your local Cadmium installation:
INCLUDECADMIUM=-I $(CADMIUM)

The CADMIUM environment variable should point to the Cadmium v2 include directory. For example:
export CADMIUM=/home/cadmium/rt_cadmium/include

2 - Compile and run all tests:
make clean && make all

3 - Run atomic and coupled model tests using shell scripts:
./scripts/run_all_tests.sh

Or run individual test scripts:
./scripts/run_control_tower_test.sh
./scripts/run_queue_test.sh
./scripts/run_runway_test.sh
./scripts/run_selector_test.sh
./scripts/run_storage_bay_test.sh
./scripts/run_merger_test.sh
./scripts/run_coupled_test.sh

These scripts compile, run, and save output to simulation_results/.

Alternatively, use make directly:
make runalltests

This runs all test suites sequentially:
- CONTROL_TOWER_TEST  (CT-1 to CT-4)
- QUEUE_TEST          (Q-1 to Q-5)
- RUNWAY_TEST         (R-1 to R-3)
- SELECTOR_TEST       (S-1 to S-5)
- STORAGE_BAY_TEST    (SB-1 to SB-2)
- MERGER_TEST         (M-1 to M-3)
- COUPLED_TEST        (SBK-1, SBK-2, H-1, H-2)

4 - Run all top-model experiments:
./scripts/run_airport_simulation.sh

Or use make:
make runexperiments

Or run individual experiments:
make run_T1    # Single plane full lifecycle
make run_T2    # Burst arrivals (6 planes at once)
make run_T3    # Staggered arrivals (50 planes over time)
make run_T4    # Rapid arrivals (5 planes, 30s apart)
make run_T5    # Boundary ID values
make run_T6    # Bay stress test (10 planes to Bay 1)

5 - Manual execution:
./bin/AIRPORT_SIMULATION input_data/T1_single_lifecycle.txt 500

Arguments: <input_file> [simulation_time]
Results are saved to simulation_results/<input_name>_output.csv

6 - View experiment results in terminal:
cat simulation_results/T1_single_lifecycle_output.csv
cat simulation_results/T2_burst_test_output.csv
cat simulation_results/T3_staggered_test_output.csv
cat simulation_results/T4_rapid_test_output.csv
cat simulation_results/T5_boundary_test_output.csv
cat simulation_results/T6_bay_stress_test_output.csv

View test results:
cat simulation_results/control_tower_output.txt
cat simulation_results/queue_output.txt
cat simulation_results/runway_output.txt
cat simulation_results/selector_output.txt
cat simulation_results/storage_bay_output.txt
cat simulation_results/merger_output.txt
cat simulation_results/coupled_output.txt
