/*
 * Test Driver for Queue Atomic Model
 *
 * Tests the FIFO queue with flow control for managing plane requests.
 * Reads test inputs from files. Format: time port value
 * Port mapping: 0=plane, 1=stop, 2=done
 *
 * Test Cases:
 *   Q-1: Single plane through queue
 *   Q-2: Multiple planes (FIFO ordering)
 *   Q-3: Stop signal pauses output
 *   Q-4: Done signal resumes output
 *   Q-5: Enqueue while stopped
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../vendor/stdout_logger.hpp"
#include <cadmium/core/modeling/coupled.hpp>
#include "../atomics/queue.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <tuple>

using namespace cadmium;

// file-based input generator state
struct QueueInputState {
    double sigma;
    double elapsed;
    size_t current_event;
    std::vector<std::tuple<double, int, int>> events;  // time port value

    explicit QueueInputState() : sigma(std::numeric_limits<double>::infinity()), elapsed(0), current_event(0) {}
};

std::ostream& operator<<(std::ostream &out, const QueueInputState& s) {
    out << "{event=" << s.current_event << ", sigma=" << s.sigma << "}";
    return out;
}

// file-based input generator for queue
class QueueInputGenerator : public Atomic<QueueInputState> {
public:
    Port<int> out_plane;
    Port<int> out_stop;
    Port<int> out_done;

    QueueInputGenerator(const std::string& id, const char* input_file) : Atomic<QueueInputState>(id, QueueInputState()) {
        out_plane = addOutPort<int>("out_plane");
        out_stop = addOutPort<int>("out_stop");
        out_done = addOutPort<int>("out_done");

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

    void internalTransition(QueueInputState& s) const override {
        s.elapsed += s.sigma;
        s.current_event++;
        if (s.current_event < s.events.size()) {
            s.sigma = std::get<0>(s.events[s.current_event]) - s.elapsed;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(QueueInputState& s, double e) const override {}

    void output(const QueueInputState& s) const override {
        if (s.current_event < s.events.size()) {
            auto& evt = s.events[s.current_event];
            int port = std::get<1>(evt);
            int value = std::get<2>(evt);
            switch (port) {
                case 0: out_plane->addMessage(value); break;
                case 1: out_stop->addMessage(value); break;
                case 2: out_done->addMessage(value); break;
            }
        }
    }

    [[nodiscard]] double timeAdvance(const QueueInputState& s) const override {
        return s.sigma;
    }
};

// test bench
class QueueTestBench : public Coupled {
public:
    QueueTestBench(const std::string& id, const char* input_file) : Coupled(id) {
        auto generator = addComponent<QueueInputGenerator>("Generator", input_file);
        auto queue = addComponent<Queue>("Queue");

        addCoupling(generator->out_plane, queue->in);
        addCoupling(generator->out_stop, queue->stop);
        addCoupling(generator->out_done, queue->done);
    }
};

void runTest(const std::string& test_id, const std::string& input_file, double sim_time) {
    std::cout << "========================================" << std::endl;
    std::cout << "Queue Test: " << test_id << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "========================================" << std::endl;

    auto model = std::make_shared<QueueTestBench>("QueueTest", input_file.c_str());
    auto rootCoordinator = RootCoordinator(model);

    rootCoordinator.setLogger(std::make_shared<STDOUTLogger>(";"));
    rootCoordinator.start();
    rootCoordinator.simulate(sim_time);
    rootCoordinator.stop();

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::string base_path = "input_data/queue/";

    if (argc > 1) {
        base_path = argv[1];
        if (base_path.back() != '/') base_path += "/";
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Queue Atomic Model Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "input format: time port value" << std::endl;
    std::cout << "  port 0=plane, 1=stop, 2=done" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // run all test cases
    runTest("Q-1", base_path + "Q1_single_plane.txt", 100.0);
    runTest("Q-2", base_path + "Q2_multiple.txt", 100.0);
    runTest("Q-3", base_path + "Q3_stop_resume.txt", 100.0);
    runTest("Q-4", base_path + "Q4_stop_empty.txt", 100.0);
    runTest("Q-5", base_path + "Q5_enqueue_stopped.txt", 100.0);

    std::cout << "========================================" << std::endl;
    std::cout << "All Queue Tests Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
