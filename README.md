# Airport Simulation - DEVS Model in Cadmium

This repository contains an airport simulation model implemented using the DEVS formalism in Cadmium.

## Repository Structure

```
Cadmium-Airport/
├── README.md
├── Airport_Simulation_Report.pdf       # Complete model documentation
├── makefile
├── atomics/                            # Atomic models
│   ├── control_tower.hpp
│   ├── queue.hpp
│   ├── runway.hpp
│   ├── selector.hpp
│   ├── storage_bay.hpp
│   └── merger.hpp
├── data_structures/                    # Custom data types
│   ├── plane_message.hpp
│   └── plane_message.cpp
├── input_data/                         # Test input files
│   ├── input_full_simulation.txt
│   └── [test files for each atomic model]
├── test/                               # Unit tests for atomic models
│   ├── main_control_tower_test.cpp
│   ├── main_queue_test.cpp
│   ├── main_runway_test.cpp
│   ├── main_selector_test.cpp
│   ├── main_storage_bay_test.cpp
│   └── main_merger_test.cpp
├── top_model/                          # Top-level coupled model
│   └── main.cpp
├── vendor/                             # Utility headers
│   ├── NDTime.hpp
│   └── iestream.hpp
├── bin/                                # Executables (auto-generated)
├── build/                              # Build artifacts (auto-generated)
└── simulation_results/                 # Simulation outputs (auto-generated)
```

## Model Overview

The airport simulation models aircraft landing and takeoff operations with **3 hierarchical levels** and **6 atomic models**.

### Atomic Models

| Model | Description |
|-------|-------------|
| **ControlTower** | Manages landing/takeoff requests, coordinates queues and runway |
| **Queue** | Buffers incoming planes (separate instances for landing and takeoff) |
| **Runway** | Handles aircraft landing and takeoff operations |
| **Selector** | Routes planes to storage bays based on plane ID ranges |
| **StorageBay** | Temporarily stores aircraft (4 instances: Bay1-4) |
| **Merger** | Combines outputs from multiple storage bays |

### Coupled Models (3 Levels)

1. **Top** - Contains ControlTower, landing_queue, takeoff_queue, Runway, and Hangar
2. **Hangar** - Contains Selector and StorageBank
3. **StorageBank** - Contains 4 StorageBay instances and Merger

### System Flow

```
External Input → landing_queue → ControlTower → Runway (land) →
Selector → StorageBay[1-4] → Merger → takeoff_queue →
ControlTower → Runway (takeoff) → External Output
```

## Setup & Installation

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- [Cadmium](https://github.com/SimulationEverywhere/cadmium) framework
- [DESTimes](https://github.com/SimulationEverywhere/DESTimes) library

### Update Include Paths

Edit the `makefile` to point to your Cadmium and DESTimes installation:

```makefile
INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include
```

Update these paths based on where you installed the libraries.

## Building the Project

```bash
# Clean previous builds and compile everything
make clean; make all
```

This will:
- Create `bin/` directory with all executables
- Create `build/` directory with object files
- Create `simulation_results/` directory for outputs

## Running Tests

### Individual Atomic Model Tests

```bash
cd bin

# Run individual tests
./CONTROL_TOWER_TEST
./QUEUE_TEST
./RUNWAY_TEST
./SELECTOR_TEST
./STORAGE_BAY_TEST
./MERGER_TEST
```

Test outputs will be saved in `simulation_results/` as:
- `{model_name}_test_output_messages.txt`
- `{model_name}_test_output_state.txt`

### Full Simulation

```bash
cd bin
./AIRPORT_SIMULATION ../input_data/input_full_simulation.txt
```

Simulation outputs will be saved as:
- `simulation_results/airport_output_messages.txt`
- `simulation_results/airport_output_state.txt`

## Input File Format

Input files contain event arrivals in the format:
```
HH:MM:SS:MS port_name value
```

Example:
```
00:01:00:00 in_landing 100
00:01:30:00 in_landing 250
00:02:00:00 in_landing 500
```

See `input_data/` folder for more examples.

## Documentation

For detailed information, see **Airport_Simulation_Report.pdf** which includes:
- DEVS formal specifications (M = <X, Y, S, δext, δint, λ, ta>)
- DEVS Graphs for all atomic models
- Coupling diagrams showing model hierarchy
- Execution traces and test results
- Model analysis and validation

## Authors

SYSC 4906G - Winter 2026

## License

Academic use only - SYSC 4906G Assignment 1
