# Airport Simulation DEVS Model

This folder contains the Airport Simulation DEVS model implemented in Cadmium.

**Repository:** https://github.com/hasibkhodayar/SYSC4906-Airport-Simulation

**Authors:** Hasib Khodayar & Hajar Assim
**Course:** SYSC 4906G, Winter 2026, Carleton University

---

## Files Organization

### Root Files
- `README.md` - This file
- `Makefile` - Compilation instructions for tests and top model
- `build_sim.sh` - Build script to compile all executables

### `atomics/`
Contains atomic model headers implemented in Cadmium:
- `controlTower.hpp` - Coordinates runway access between landing and takeoff
- `queue.hpp` - FIFO buffer with stop/done flow control
- `runway.hpp` - Processes aircraft landing/takeoff operations
- `selector.hpp` - Routes planes to storage bays based on ID
- `storageBay.hpp` - Stores planes, drains to merger
- `merger.hpp` - Combines outputs from 4 bays into single stream

### `coupled/`
Contains coupled model headers:
- `hangar.hpp` - Top-level hangar: Selector + StorageBank
- `storageBank.hpp` - Internal coupled model: 4 Bays + Merger

### `data_structures/`
- `plane_message.hpp`
- `plane_message.cpp`

### `bin/`
Created automatically during compilation. Contains executables.

### `build/`
Created automatically during compilation. Contains `.o` build files.

### `input_data/`
Contains all input event trajectories for simulation:
- `control_tower/` - CT-1 to CT-4: ControlTower atomic tests
- `queue/` - Q-1 to Q-5: Queue atomic tests
- `runway/` - R-1 to R-3: Runway atomic tests
- `selector/` - S-1 to S-5: Selector atomic tests
- `storage_bay/` - SB-1 to SB-2: StorageBay atomic tests
- `merger/` - M-1 to M-3: Merger atomic tests
- `storage_bank/` - SBK-1 to SBK-2: StorageBank coupled tests
- `hangar/` - H-1 to H-2: Hangar coupled tests
- `T1-T6_*.txt` - Top model experiment input files

### `scripts/`
Contains shell scripts to compile and run each test/simulation:
- `run_all_tests.sh` - Compiles and runs all atomic + coupled tests
- `run_control_tower_test.sh`
- `run_queue_test.sh`
- `run_runway_test.sh`
- `run_selector_test.sh`
- `run_storage_bay_test.sh`
- `run_merger_test.sh`
- `run_coupled_test.sh`
- `run_airport_simulation.sh` - Compiles and runs all T1-T6 experiments

### `test/`
Contains test driver files for atomic and coupled models:
- `main_control_tower_test.cpp`
- `main_queue_test.cpp`
- `main_runway_test.cpp`
- `main_selector_test.cpp`
- `main_storage_bay_test.cpp`
- `main_merger_test.cpp`
- `main_coupled_test.cpp` - StorageBank and Hangar coupled tests

### `top_model/`
Contains the integrated Airport Simulation driver:
- `main.cpp`

### `simulation_results/`
Stores execution logs from simulations. Output CSV files are generated here automatically when experiments are run.

---

## Build Instructions

### Prerequisites
1. Cadmium v2 library installed
2. Set the `CADMIUM` environment variable to point to the Cadmium include directory:
   ```bash
   export CADMIUM=/path/to/cadmium_v2/include
   ```
   Example:
   ```bash
   export CADMIUM=/Users/username/cadmium_v2/include
   ```

### Build the Project
Simply run the build script:
```bash
./build_sim.sh
```

This will:
- Clean previous builds
- Compile all atomic and coupled model tests
- Compile the main airport simulation
- Create executables in the `bin/` directory

---

## Running the Simulation

### Run All Experiments
```bash
make runexperiments
```

This runs all top-model experiments (T1-T6):
- **T1:** Single plane full lifecycle
- **T2:** Burst arrivals (6 planes at once)
- **T3:** Staggered arrivals (50 planes over time)
- **T4:** Rapid arrivals (5 planes, 30s apart)
- **T5:** Boundary ID values
- **T6:** Bay stress test (10 planes to Bay 1)

### Run Individual Experiments
```bash
make run_T1    # Single plane full lifecycle
make run_T2    # Burst arrivals
make run_T3    # Staggered arrivals
make run_T4    # Rapid arrivals
make run_T5    # Boundary ID values
make run_T6    # Bay stress test
```

### Manual Execution
```bash
./bin/AIRPORT_SIMULATION input_data/T1_single_lifecycle.txt 500
```

**Arguments:** `<input_file> [simulation_time]`
**Output:** Results are saved to `simulation_results/<input_name>_output.csv`

---

## Running Tests

### Run All Tests
```bash
make runalltests
```

Or using the shell script:
```bash
./scripts/run_all_tests.sh
```

This runs all test suites sequentially:
- `CONTROL_TOWER_TEST` (CT-1 to CT-4)
- `QUEUE_TEST` (Q-1 to Q-5)
- `RUNWAY_TEST` (R-1 to R-3)
- `SELECTOR_TEST` (S-1 to S-5)
- `STORAGE_BAY_TEST` (SB-1 to SB-2)
- `MERGER_TEST` (M-1 to M-3)
- `COUPLED_TEST` (SBK-1, SBK-2, H-1, H-2)

### Run Individual Tests
```bash
./scripts/run_control_tower_test.sh
./scripts/run_queue_test.sh
./scripts/run_runway_test.sh
./scripts/run_selector_test.sh
./scripts/run_storage_bay_test.sh
./scripts/run_merger_test.sh
./scripts/run_coupled_test.sh
```

These scripts compile, run, and save output to `simulation_results/`.

---

## Viewing Results

### View Experiment Results
```bash
cat simulation_results/T1_single_lifecycle_output.csv
cat simulation_results/T2_burst_test_output.csv
cat simulation_results/T3_staggered_test_output.csv
cat simulation_results/T4_rapid_test_output.csv
cat simulation_results/T5_boundary_test_output.csv
cat simulation_results/T6_bay_stress_test_output.csv
```

### View Test Results
```bash
cat simulation_results/control_tower_output.txt
cat simulation_results/queue_output.txt
cat simulation_results/runway_output.txt
cat simulation_results/selector_output.txt
cat simulation_results/storage_bay_output.txt
cat simulation_results/merger_output.txt
cat simulation_results/coupled_output.txt
```
