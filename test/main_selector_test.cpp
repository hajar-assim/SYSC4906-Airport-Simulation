/*
 * Test Driver for Selector Atomic Model
 *
 * Tests plane routing to storage bays based on plane ID ranges.
 * Uses IEStream to read test inputs from files.
 * Verifies Bug #4 fix - corrected boundaries (249, 499, 749, 999).
 *
 * Test Cases:
 *   S-1: Route to Bay 1 (ID <= 249)
 *   S-2: Route to Bay 2 (250 <= ID <= 499)
 *   S-3: Route to Bay 3 (500 <= ID <= 749)
 *   S-4: Route to Bay 4 (750 <= ID <= 999)
 *   S-5: Boundary values test
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/lib/iestream.hpp>
#include "../atomics/selector.hpp"
#include <iostream>
#include <string>

using namespace cadmium;

// test bench using iestream
class SelectorTestBench : public Coupled {
public:
    SelectorTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<lib::IEStream<int>>("Generator", input_file);
        auto selector = addComponent<Selector>("Selector");

        addCoupling(generator->out, selector->in);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "Selector Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<SelectorTestBench>("SelectorTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/selector/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Selector Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "bay boundaries (bug #4 fix):" << std::endl;
    std::cout << "  bay 1: 0-249, bay 2: 250-499" << std::endl;
    std::cout << "  bay 3: 500-749, bay 4: 750-999" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("S-1", base_path + "S1_bay1.txt", 100.0);
    runTest("S-2", base_path + "S2_bay2.txt", 100.0);
    runTest("S-3", base_path + "S3_bay3.txt", 100.0);
    runTest("S-4", base_path + "S4_bay4.txt", 100.0);
    runTest("S-5", base_path + "S5_boundary.txt", 400.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All Selector Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
