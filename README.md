# Airport Simulation - DEVS Model in Cadmium

A Cadmium v2 implementation of an airport landing and takeoff simulation using the DEVS (Discrete Event System Specification) formalism. This project ports the original CD++ model by Luciano Burotti and Luis Fernando De Simoni to Cadmium.

## Authors

- **Hasib Khodayar**
- **Hajar Assim**

Course: SYSC 4906G, Winter 2026, Carleton University

## Repository Structure

```
SYSC4906-Airport-Simulation/
├── README.md
├── makefile
├── atomics/                            # Atomic DEVS models
│   ├── controlTower.hpp
│   ├── queue.hpp
│   ├── runway.hpp
│   ├── selector.hpp
│   ├── storageBay.hpp
│   └── merger.hpp
├── coupled/                            # Coupled DEVS models
│   ├── storageBank.hpp
│   └── hangar.hpp
├── data_structures/
│   ├── plane_message.hpp
│   └── plane_message.cpp
├── input_data/                         # Test input files
│   ├── control_tower/                  # CT-1 to CT-4
│   ├── queue/                          # Q-1 to Q-5
│   ├── runway/                         # R-1 to R-3
│   ├── selector/                       # S-1 to S-5
│   ├── storage_bay/                    # SB-1 to SB-2
│   ├── merger/                         # M-1 to M-3
│   ├── storage_bank/                   # SBK-1 to SBK-2
│   ├── hangar/                         # H-1 to H-2
│   └── T1-T6_*.txt                     # Top model experiments
├── test/                               # Test drivers
│   ├── main_control_tower_test.cpp
│   ├── main_queue_test.cpp
│   ├── main_runway_test.cpp
│   ├── main_selector_test.cpp
│   ├── main_storage_bay_test.cpp
│   ├── main_merger_test.cpp
│   └── main_coupled_test.cpp
├── top_model/
│   └── main.cpp                        # Main simulation driver
├── bin/                                # Executables (generated)
├── build/                              # Object files (generated)
└── simulation_results/                 # Output CSV files (generated)
```

## Model Overview

The airport simulation models aircraft landing and takeoff operations using a 3-level coupled model hierarchy with 6 atomic models.

### Atomic Models

| Model | States | Timing | Description |
|-------|--------|--------|-------------|
| **ControlTower** | IDLE, SIGNAL, WAIT | σ=0 (SIGNAL), σ=60s (WAIT) | Coordinates runway access between landing and takeoff queues |
| **Queue** | IDLE, SENDING, WAIT_ACK | σ=0 (SENDING) | FIFO buffer with stop/done flow control |
| **Runway** | IDLE, LANDING, TAKEOFF | σ=60s (operations) | Processes aircraft landing/takeoff |
| **Selector** | IDLE, ROUTING | σ=30s (routing delay) | Routes planes to bays based on ID |
| **StorageBay** | IDLE, ACTIVE | σ=0 (immediate drain) | Stores planes, drains to merger |
| **Merger** | IDLE, ACTIVE | σ=0 (immediate output) | Combines outputs from 4 bays |

### Bay Assignment (Bug #4 Fix)

The Selector routes planes to storage bays based on plane ID:

| Bay | ID Range | Condition |
|-----|----------|-----------|
| Bay 1 | 0-249 | id ≤ 249 |
| Bay 2 | 250-499 | id ≤ 499 |
| Bay 3 | 500-749 | id ≤ 749 |
| Bay 4 | 750-999 | id ≤ 999 |

### Coupled Model Hierarchy

```
AirportTop
├── landing_queue (Queue)
├── takeoff_queue (Queue)
├── ControlTower
├── Runway
└── Hangar
    ├── Selector
    └── StorageBank
        ├── Bay1 (StorageBay)
        ├── Bay2 (StorageBay)
        ├── Bay3 (StorageBay)
        ├── Bay4 (StorageBay)
        └── Merger
```

### Data Flow

```
Arriving Plane → landing_queue → ControlTower → Runway (LANDING)
                                      ↓
                              Hangar/Selector → Bay[1-4] → Merger
                                      ↓
                    ControlTower ← takeoff_queue ←
                          ↓
                    Runway (TAKEOFF) → Departed Plane
```

## Prerequisites

- C++17 compatible compiler (g++ 8+ recommended)
- [Cadmium v2](https://github.com/SimulationEverywhere/cadmium_v2) framework

## Setup

1. Clone this repository

2. Clone Cadmium v2 (if not already installed):
```bash
cd ~
git clone https://github.com/SimulationEverywhere/cadmium_v2.git
```

3. The makefile defaults to `~/cadmium_v2`. If your Cadmium is installed elsewhere, set the `CADMIUM_HOME` environment variable:
```bash
export CADMIUM_HOME=/path/to/your/cadmium_v2
```

Or edit the makefile directly:
```makefile
CADMIUM_HOME ?= /your/custom/path
```

## Building

```bash
# Clean and build everything
make clean && make all
```

This creates:
- `bin/AIRPORT_SIMULATION` - Main simulation executable
- `bin/CONTROL_TOWER_TEST` - ControlTower atomic tests
- `bin/QUEUE_TEST` - Queue atomic tests
- `bin/RUNWAY_TEST` - Runway atomic tests
- `bin/SELECTOR_TEST` - Selector atomic tests
- `bin/STORAGE_BAY_TEST` - StorageBay atomic tests
- `bin/MERGER_TEST` - Merger atomic tests
- `bin/COUPLED_TEST` - StorageBank and Hangar coupled tests

## Running Tests

### Run All Tests

```bash
make runalltests
```

This runs all atomic and coupled model tests sequentially.

### Run Individual Test Suites

```bash
cd bin

./CONTROL_TOWER_TEST    # CT-1 to CT-4
./QUEUE_TEST            # Q-1 to Q-5
./RUNWAY_TEST           # R-1 to R-3
./SELECTOR_TEST         # S-1 to S-5
./STORAGE_BAY_TEST      # SB-1 to SB-2
./MERGER_TEST           # M-1 to M-3
./COUPLED_TEST          # SBK-1, SBK-2, H-1, H-2
```

### Test Cases

| Test ID | Model | Description |
|---------|-------|-------------|
| CT-1 | ControlTower | Single landing request |
| CT-2 | ControlTower | Single takeoff request |
| CT-3 | ControlTower | Back-to-back operations |
| CT-4 | ControlTower | Request while busy |
| Q-1 | Queue | Single plane enqueue/dequeue |
| Q-2 | Queue | Multiple planes |
| Q-3 | Queue | Stop and resume |
| Q-4 | Queue | Stop while empty |
| Q-5 | Queue | Enqueue while stopped |
| R-1 | Runway | Landing operation |
| R-2 | Runway | Takeoff operation |
| R-3 | Runway | Sequential operations |
| S-1 to S-4 | Selector | Route to Bay 1-4 |
| S-5 | Selector | Boundary ID values (249, 250, 499, 500, 749, 750) |
| SB-1 | StorageBay | Single plane |
| SB-2 | StorageBay | FIFO ordering |
| M-1 | Merger | Single input |
| M-2 | Merger | All ports sequentially |
| M-3 | Merger | Simultaneous inputs |
| SBK-1 | StorageBank | Single bay routing |
| SBK-2 | StorageBank | All bays |
| H-1 | Hangar | Route and store |
| H-2 | Hangar | All bays routing |

## Running Experiments

### Run All Experiments

```bash
make runexperiments
```

### Run Individual Experiments

```bash
make run_T1    # Single plane full lifecycle
make run_T2    # Burst arrivals (6 planes at once)
make run_T3    # Staggered arrivals (50 planes over time)
make run_T4    # Rapid arrivals (5 planes, 30s apart)
make run_T5    # Boundary ID values
make run_T6    # Bay stress test (10 planes to Bay 1)
```

### Manual Execution

```bash
cd bin
./AIRPORT_SIMULATION ../input_data/T1_single_lifecycle.txt 500
```

Arguments:
- `input_file` - Path to input file
- `simulation_time` - Maximum simulation time in seconds (default: 36000)

Results are saved to `simulation_results/{input_name}_output.csv`

## Input File Format

### Single-Port Models (Selector, StorageBay)

```
time value
```

Example (`S1_bay1.txt`):
```
10 100
```
Plane 100 arrives at t=10s

### Multi-Port Models (ControlTower, Queue, Runway, Merger)

```
time port value
```

Example (`CT1_single_landing.txt`):
```
60 0 5
```
Plane 5 arrives on port 0 (in_landing) at t=60s

Port mappings:
- **ControlTower**: 0=in_landing, 1=in_takeoff
- **Queue**: 0=plane, 1=stop, 2=done
- **Runway**: 0=land, 1=takeoff
- **Merger**: 1=in1, 2=in2, 3=in3, 4=in4

## Output Format

Simulation output is CSV with columns:
```
time;model_id;model_name;port_name;data
```

Example:
```
60;1;ControlTower;land;5
120;1;ControlTower;done_landing;1
```

