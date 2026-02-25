/*
 * Test Driver for ControlTower Atomic Model
 *
 * Tests the air traffic control tower's handling of landing and takeoff requests.
 * Reads test inputs from files. Format: time port value
 * Port mapping: 0=landing, 1=takeoff
 *
 * Test Cases:
 *   CT-1: Single landing request
 *   CT-2: Single takeoff request
 *   CT-3: Sequential landing then takeoff
 *   CT-4: Concurrent requests (second ignored while busy)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../vendor/stdout_logger.hpp"
#include <cadmium/core/modeling/coupled.hpp>
#include "../atomics/controlTower.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <tuple>

using namespace cadmium;

// file-based input generator state
struct TowerInputState {
    double sigma;
    double elapsed;
    size_t current_event;
    std::vector<std::tuple<double, int, int>> events;  // time port value

    explicit TowerInputState() : sigma(std::numeric_limits<double>::infinity()), elapsed(0), current_event(0) {}
};

std::ostream& operator<<(std::ostream &out, const TowerInputState& s) {
    out << "{event=" << s.current_event << ", sigma=" << s.sigma << "}";
    return out;
}

// file-based input generator for control tower
class TowerInputGenerator : public Atomic<TowerInputState> {
public:
    Port<int> out_landing;
    Port<int> out_takeoff;

    TowerInputGenerator(const std::string& id, const char* input_file) : Atomic<TowerInputState>(id, TowerInputState()) {
        out_landing = addOutPort<int>("out_landing");
        out_takeoff = addOutPort<int>("out_takeoff");

        // read events from file
        std::ifstream file(input_file);
        double time;
        int port, value;
        while (file >> time >> port >> value) {
            state.events.push_back({time, port, value});
        }

        if (!state.events.empty()) {
            state.sigma = std::get<0>(state.events[0]);
        }
    }

    void internalTransition(TowerInputState& s) const override {
        s.elapsed += s.sigma;
        s.current_event++;
        if (s.current_event < s.events.size()) {
            s.sigma = std::get<0>(s.events[s.current_event]) - s.elapsed;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(TowerInputState& s, double e) const override {}

    void output(const TowerInputState& s) const override {
        if (s.current_event < s.events.size()) {
            auto& evt = s.events[s.current_event];
            int port = std::get<1>(evt);
            int value = std::get<2>(evt);
            if (port == 0)
                out_landing->addMessage(value);
            else
                out_takeoff->addMessage(value);
        }
    }

    [[nodiscard]] double timeAdvance(const TowerInputState& s) const override {
        return s.sigma;
    }
};

// test bench
class TowerTestBench : public Coupled {
public:
    TowerTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<TowerInputGenerator>("Generator", input_file);
        auto tower = addComponent<ControlTower>("ControlTower");

        addCoupling(generator->out_landing, tower->in_landing);
        addCoupling(generator->out_takeoff, tower->in_takeoff);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "ControlTower Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<TowerTestBench>("TowerTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger(std::make_shared<STDOUTLogger>(";"));
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/control_tower/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "ControlTower Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "input format: time port value" << std::endl;
    std::cout << "  port 0=landing, 1=takeoff" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("CT-1", base_path + "CT1_single_landing.txt", 300.0);
    runTest("CT-2", base_path + "CT2_single_takeoff.txt", 300.0);
    runTest("CT-3", base_path + "CT3_back_to_back.txt", 400.0);
    runTest("CT-4", base_path + "CT4_while_busy.txt", 300.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All ControlTower Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
