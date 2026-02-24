/*
 * Test Driver for Merger Atomic Model
 *
 * Tests combining outputs from four storage bays into a single stream.
 * Reads test inputs from files. Format: time port value
 * Port mapping: 1=in1, 2=in2, 3=in3, 4=in4
 *
 * Test Cases:
 *   M-1: Single input from in1
 *   M-2: Inputs from all ports
 *   M-3: Simultaneous inputs
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/merger.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <tuple>

using namespace cadmium;

// file-based input generator state
struct MergerInputState {
    double sigma;
    double elapsed;
    size_t current_event;
    std::vector<std::tuple<double, int, int>> events;

    explicit MergerInputState() : sigma(std::numeric_limits<double>::infinity()), elapsed(0), current_event(0) {}
};

std::ostream& operator<<(std::ostream &out, const MergerInputState& s) {
    out << "{event=" << s.current_event << ", sigma=" << s.sigma << "}";
    return out;
}

// file-based input generator for merger
class MergerInputGenerator : public Atomic<MergerInputState> {
public:
    Port<int> out1;
    Port<int> out2;
    Port<int> out3;
    Port<int> out4;

    MergerInputGenerator(const std::string& id, const char* input_file) : Atomic<MergerInputState>(id, MergerInputState()) {
        out1 = addOutPort<int>("out1");
        out2 = addOutPort<int>("out2");
        out3 = addOutPort<int>("out3");
        out4 = addOutPort<int>("out4");

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

    void internalTransition(MergerInputState& s) const override {
        s.elapsed += s.sigma;
        s.current_event++;
        if (s.current_event < s.events.size()) {
            s.sigma = std::get<0>(s.events[s.current_event]) - s.elapsed;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(MergerInputState& s, double e) const override {}

    void output(const MergerInputState& s) const override {
        if (s.current_event < s.events.size()) {
            auto& evt = s.events[s.current_event];
            int port = std::get<1>(evt);
            int value = std::get<2>(evt);
            switch (port) {
                case 1: out1->addMessage(value); break;
                case 2: out2->addMessage(value); break;
                case 3: out3->addMessage(value); break;
                case 4: out4->addMessage(value); break;
            }
        }
    }

    [[nodiscard]] double timeAdvance(const MergerInputState& s) const override {
        return s.sigma;
    }
};

// test bench
class MergerTestBench : public Coupled {
public:
    MergerTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<MergerInputGenerator>("Generator", input_file);
        auto merger = addComponent<Merger>("Merger");

        addCoupling(generator->out1, merger->in1);
        addCoupling(generator->out2, merger->in2);
        addCoupling(generator->out3, merger->in3);
        addCoupling(generator->out4, merger->in4);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "Merger Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<MergerTestBench>("MergerTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/merger/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Merger Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "input format: time port value" << std::endl;
    std::cout << "  port 1=in1, 2=in2, 3=in3, 4=in4" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("M-1", base_path + "M1_single.txt", 50.0);
    runTest("M-2", base_path + "M2_all_ports.txt", 100.0);
    runTest("M-3", base_path + "M3_simultaneous.txt", 50.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All Merger Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
