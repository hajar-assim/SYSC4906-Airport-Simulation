/*
 * Test Driver for StorageBay Atomic Model
 *
 * Tests the FIFO buffer storage and immediate draining behavior.
 * Uses IEStream to read test inputs from files.
 *
 * Test Cases:
 *   SB-1: Single plane storage and output
 *   SB-2: Multiple planes (FIFO order)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../vendor/stdout_logger.hpp"
#include <cadmium/core/modeling/coupled.hpp>
#include <fstream>
#include <cadmium/lib/iestream.hpp>
#include "../atomics/storageBay.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace cadmium;

// test bench using iestream
class StorageBayTestBench : public Coupled {
public:
    StorageBayTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<lib::IEStream<int>>("Generator", input_file);
        auto bay = addComponent<StorageBay>("StorageBay");

        addCoupling(generator->out, bay->in);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "StorageBay Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<StorageBayTestBench>("StorageBayTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger(std::make_shared<STDOUTLogger>(";"));
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/storage_bay/";

    // allow override of base path
    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "StorageBay Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("SB-1", base_path + "SB1_single.txt", 50.0);
    runTest("SB-2", base_path + "SB2_fifo.txt", 50.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All StorageBay Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
