/*
 * Test Driver for Runway Atomic Model
 *
 * Tests the runway processing of landing and takeoff operations.
 * Reads test inputs from files. Format: time port value
 * Port mapping: 0=land, 1=takeoff
 *
 * Test Cases:
 *   R-1: Single landing operation
 *   R-2: Single takeoff operation
 *   R-3: Landing then takeoff sequence
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/runway.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <tuple>

using namespace cadmium;

// file-based input generator state
struct RunwayInputState {
    double sigma;
    double elapsed;
    size_t current_event;
    std::vector<std::tuple<double, int, int>> events;

    explicit RunwayInputState() : sigma(std::numeric_limits<double>::infinity()), elapsed(0), current_event(0) {}
};

std::ostream& operator<<(std::ostream &out, const RunwayInputState& s) {
    out << "{event=" << s.current_event << ", sigma=" << s.sigma << "}";
    return out;
}

// file-based input generator for runway
class RunwayInputGenerator : public Atomic<RunwayInputState> {
public:
    Port<int> out_land;
    Port<int> out_takeoff;

    RunwayInputGenerator(const std::string& id, const char* input_file) : Atomic<RunwayInputState>(id, RunwayInputState()) {
        out_land = addOutPort<int>("out_land");
        out_takeoff = addOutPort<int>("out_takeoff");

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

    void internalTransition(RunwayInputState& s) const override {
        s.elapsed += s.sigma;
        s.current_event++;
        if (s.current_event < s.events.size()) {
            s.sigma = std::get<0>(s.events[s.current_event]) - s.elapsed;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(RunwayInputState& s, double e) const override {}

    void output(const RunwayInputState& s) const override {
        if (s.current_event < s.events.size()) {
            auto& evt = s.events[s.current_event];
            int port = std::get<1>(evt);
            int value = std::get<2>(evt);
            if (port == 0)
                out_land->addMessage(value);
            else
                out_takeoff->addMessage(value);
        }
    }

    [[nodiscard]] double timeAdvance(const RunwayInputState& s) const override {
        return s.sigma;
    }
};

// test bench
class RunwayTestBench : public Coupled {
public:
    RunwayTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<RunwayInputGenerator>("Generator", input_file);
        auto runway = addComponent<Runway>("Runway");

        addCoupling(generator->out_land, runway->land);
        addCoupling(generator->out_takeoff, runway->takeoff);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "Runway Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<RunwayTestBench>("RunwayTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/runway/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Runway Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "input format: time port value" << std::endl;
    std::cout << "  port 0=land, 1=takeoff" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("R-1", base_path + "R1_landing.txt", 200.0);
    runTest("R-2", base_path + "R2_takeoff.txt", 200.0);
    runTest("R-3", base_path + "R3_sequential.txt", 250.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All Runway Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
