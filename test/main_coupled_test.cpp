/*
 * Test Driver for Coupled Models
 *
 * Tests the integrated behavior of coupled models:
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
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

using namespace cadmium;

// multi-port generator for StorageBank (reads time port value format)
struct StorageBankGenState {
    double sigma;
    double elapsed;
    size_t current_event;
    std::vector<std::tuple<double, int, int>> events;  // time port value

    explicit StorageBankGenState() : sigma(std::numeric_limits<double>::infinity()), elapsed(0), current_event(0) {}
};

std::ostream& operator<<(std::ostream &out, const StorageBankGenState& s) {
    out << "{event=" << s.current_event << ", sigma=" << s.sigma << "}";
    return out;
}

class StorageBankGenerator : public Atomic<StorageBankGenState> {
public:
    Port<int> out1;
    Port<int> out2;
    Port<int> out3;
    Port<int> out4;

    StorageBankGenerator(const std::string& id, const char* input_file) : Atomic<StorageBankGenState>(id, StorageBankGenState()) {
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

    void internalTransition(StorageBankGenState& s) const override {
        s.elapsed += s.sigma;
        s.current_event++;
        if (s.current_event < s.events.size()) {
            s.sigma = std::get<0>(s.events[s.current_event]) - s.elapsed;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(StorageBankGenState& s, double e) const override {}

    void output(const StorageBankGenState& s) const override {
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

    [[nodiscard]] double timeAdvance(const StorageBankGenState& s) const override {
        return s.sigma;
    }
};

// test bench using multi-port generator
class StorageBankTestBench : public Coupled {
public:
    StorageBankTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<StorageBankGenerator>("Generator", input_file);
        auto bank = addComponent<StorageBank>("StorageBank");

        addCoupling(generator->out1, bank->in1);
        addCoupling(generator->out2, bank->in2);
        addCoupling(generator->out3, bank->in3);
        addCoupling(generator->out4, bank->in4);
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
    std::cout << "StorageBank input: time port value (port 1-4)" << std::endl;
    std::cout << "Hangar input: time value (selector routes by ID)" << std::endl;
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
