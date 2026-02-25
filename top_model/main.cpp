/*
 * Airport Simulation - Top Model
 *
 * Main driver for the airport DEVS simulation. Uses IEStream to read
 * plane arrivals from input file and runs the full landing-storage-takeoff cycle.
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../vendor/stdout_logger.hpp"
#include <cadmium/core/logger/csv.hpp>
#include <cadmium/core/modeling/coupled.hpp>
#include <cadmium/lib/iestream.hpp>

#include "../atomics/controlTower.hpp"
#include "../atomics/queue.hpp"
#include "../atomics/runway.hpp"
#include "../coupled/hangar.hpp"

#include <iostream>
#include <limits>

using namespace cadmium;

// airporttop the main airport model with queues tower runway hangar
class AirportTop : public Coupled {
public:
    Port<int> in_landing;
    Port<int> out_takeoff;

    AirportTop(const std::string& id) : Coupled(id) {
        in_landing = addInPort<int>("in_landing");
        out_takeoff = addOutPort<int>("out_takeoff");

        auto tower = addComponent<ControlTower>("ControlTower");
        auto landing_queue = addComponent<Queue>("landing_queue");
        auto takeoff_queue = addComponent<Queue>("takeoff_queue");
        auto runway = addComponent<Runway>("Runway");
        auto hangar = addComponent<Hangar>("Hangar");

        // planes come in through landing queue
        addCoupling(in_landing, landing_queue->in);

        // queues send planes to tower
        addCoupling(landing_queue->out, tower->in_landing);
        addCoupling(takeoff_queue->out, tower->in_takeoff);

        // tower controls the queues with stop done signals
        addCoupling(tower->stop_landing, landing_queue->stop);
        addCoupling(tower->stop_takeoff, takeoff_queue->stop);
        addCoupling(tower->done_landing, landing_queue->done);
        addCoupling(tower->done_takeoff, takeoff_queue->done);

        // tower sends planes to runway
        addCoupling(tower->land, runway->land);
        addCoupling(tower->takeoff, runway->takeoff);

        // after landing planes go to hangar then back to takeoff queue
        addCoupling(runway->landing_exit, hangar->in);
        addCoupling(hangar->hangar_exit, takeoff_queue->in);

        // takeoff exit is the final output
        addCoupling(runway->takeoff_exit, out_takeoff);
    }
};

// test bench uses iestream for input

class AirportSimulation : public Coupled {
public:
    Port<int> out_takeoff;

    AirportSimulation(const std::string& id, const char* input_file) : Coupled(id) {
        out_takeoff = addOutPort<int>("out_takeoff");

        // iestream reads the input file and generates plane arrivals
        auto generator = addComponent<lib::IEStream<int>>("Generator", input_file);
        auto airport = addComponent<AirportTop>("Airport");

        addCoupling(generator->out, airport->in_landing);
        addCoupling(airport->out_takeoff, out_takeoff);
    }
};

// helper to extract test name from input file path
std::string getTestName(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    size_t lastDot = path.find_last_of(".");
    if (lastSlash == std::string::npos) lastSlash = 0;
    else lastSlash++;
    if (lastDot == std::string::npos || lastDot < lastSlash) lastDot = path.length();
    return path.substr(lastSlash, lastDot - lastSlash);
}

// main

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input_file> [simulation_time]" << std::endl;
        std::cout << "Input format: time plane_id (one per line)" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    double sim_time = (argc > 2) ? std::stod(argv[2]) : 36000.0;

    // generate output filename from input filename
    std::string test_name = getTestName(input_file);
    std::string output_file = "simulation_results/" + test_name + "_output.csv";

    std::cout << "========================================" << std::endl;
    std::cout << "Airport Simulation Starting" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "Output file: " << output_file << std::endl;
    std::cout << "Simulation time: " << sim_time << " seconds" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    auto model = std::make_shared<AirportSimulation>("AirportSimulation", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    // log to csv file
    rootCoordinator.setLogger(std::make_shared<CSVLogger>(output_file, ";"));
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << "Simulation complete. Results saved to: " << output_file << std::endl;

    return 0;
}
