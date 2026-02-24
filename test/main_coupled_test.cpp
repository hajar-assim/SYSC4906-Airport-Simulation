/*
 * Test Driver for Coupled Models
 *
 * Tests the integrated behavior of coupled models using IEStream input files:
 * - StorageBank (4 bays + merger): SBK-1, SBK-2
 * - Hangar (selector + storage bank): H-1, H-2
 *
 * Note: AirportTop tests are run via top_model experiments (T1-T6)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/lib/iestream.hpp>

#include "../coupled/hangar.hpp"

#include <iostream>
#include <string>

using namespace cadmium;

// test benches using iestream

class StorageBankTestBench : public Coupled {
public:
    StorageBankTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<lib::IEStream<int>>("Generator", input_file);
        auto bank = addComponent<StorageBank>("StorageBank");

        // route all inputs to in1 for single bay test
        addCoupling(generator->out, bank->in1);
    }
};

class HangarTestBench : public Coupled {
public:
    HangarTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<lib::IEStream<int>>("Generator", input_file);
        auto hangar = addComponent<Hangar>("Hangar");

        addCoupling(generator->out, hangar->in);
    }
};

void runStorageBankTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "StorageBank Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<StorageBankTestBench>("StorageBankTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

void runHangarTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "Hangar Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<HangarTestBench>("HangarTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Coupled Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // storagebank tests
    runStorageBankTest("SBK-1", base_path + "storage_bank/SBK1_one_bay.txt", 50.0);
    runStorageBankTest("SBK-2", base_path + "storage_bank/SBK2_all_bays.txt", 100.0);

    // hangar tests
    runHangarTest("H-1", base_path + "hangar/H1_route_store.txt", 100.0);
    runHangarTest("H-2", base_path + "hangar/H2_all_bays.txt", 250.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All Coupled Model Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
