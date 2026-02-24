/*
 * Selector Atomic Model
 *
 * Routes planes to one of 4 storage bays based on plane ID.
 * Takes 30 seconds to route each plane.
 *
 * Bay assignment (fixed from original buggy boundaries):
 *   Bay 1: ID 0-249
 *   Bay 2: ID 250-499
 *   Bay 3: ID 500-749
 *   Bay 4: ID 750-999
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <queue>
#include <limits>

using namespace cadmium;

enum class SelectorPhase { IDLE, ROUTING };

struct SelectorState {
    SelectorPhase phase;
    std::queue<int> pending_planes;
    int current_plane;
    double sigma;

    static constexpr double ROUTING_TIME = 30.0;  // 30 seconds to route

    // Bay boundaries - fixed from original bug
    static constexpr int BAY1_MAX = 249;
    static constexpr int BAY2_MAX = 499;
    static constexpr int BAY3_MAX = 749;
    static constexpr int BAY4_MAX = 999;

    explicit SelectorState()
        : phase(SelectorPhase::IDLE),
          current_plane(0),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const SelectorState& state) {
    std::string phaseStr = (state.phase == SelectorPhase::IDLE) ? "IDLE" : "ROUTING";
    out << "{phase=" << phaseStr << ", plane=" << state.current_plane
        << ", pending=" << state.pending_planes.size() << ", sigma=" << state.sigma << "}";
    return out;
}

class Selector : public Atomic<SelectorState> {
public:
    Port<int> in;
    Port<int> out1;  // bay 1
    Port<int> out2;  // bay 2
    Port<int> out3;  // bay 3
    Port<int> out4;  // bay 4

    Selector(const std::string& id) : Atomic<SelectorState>(id, SelectorState()) {
        in = addInPort<int>("in");
        out1 = addOutPort<int>("out1");
        out2 = addOutPort<int>("out2");
        out3 = addOutPort<int>("out3");
        out4 = addOutPort<int>("out4");
    }

    // check if more planes waiting
    void internalTransition(SelectorState& state) const override {
        if (!state.pending_planes.empty()) {
            // more planes to route
            state.current_plane = state.pending_planes.front();
            state.pending_planes.pop();
            state.sigma = SelectorState::ROUTING_TIME;
        } else {
            // done go idle
            state.phase = SelectorPhase::IDLE;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // new plane arrives
    void externalTransition(SelectorState& state, double e) const override {
        if (state.phase == SelectorPhase::ROUTING) {
            state.sigma -= e;
        }

        for (const auto& plane_id : in->getBag()) {
            if (state.phase == SelectorPhase::IDLE) {
                // start routing right away
                state.current_plane = plane_id;
                state.phase = SelectorPhase::ROUTING;
                state.sigma = SelectorState::ROUTING_TIME;
            } else {
                // already busy queue it
                state.pending_planes.push(plane_id);
            }
        }
    }

    // send plane to correct bay
    void output(const SelectorState& state) const override {
        if (state.phase != SelectorPhase::ROUTING) return;

        int id = state.current_plane;

        if (id <= SelectorState::BAY1_MAX) {
            out1->addMessage(id);
        } else if (id <= SelectorState::BAY2_MAX) {
            out2->addMessage(id);
        } else if (id <= SelectorState::BAY3_MAX) {
            out3->addMessage(id);
        } else if (id <= SelectorState::BAY4_MAX) {
            out4->addMessage(id);
        } else {
            // out of range send to bay 4 anyway
            std::cerr << "Warning: Plane ID " << id << " out of range, sending to Bay 4" << std::endl;
            out4->addMessage(id);
        }
    }

    [[nodiscard]] double timeAdvance(const SelectorState& state) const override {
        return state.sigma;
    }
};

#endif
