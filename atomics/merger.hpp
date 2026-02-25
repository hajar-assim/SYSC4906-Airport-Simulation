/*
 * Merger Atomic Model
 *
 * Combines planes from all 4 bays into a single output stream.
 * Same as storage bay - planes come in and go out immediately.
 *
 * States: IDLE, ACTIVE
 *   - IDLE: No planes
 *   - ACTIVE: Outputting planes (Ta = 0)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef MERGER_HPP
#define MERGER_HPP

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <queue>
#include <limits>

using namespace cadmium;

enum class MergerPhase { IDLE, ACTIVE };

struct MergerState {
    MergerPhase phase;
    std::queue<int> elements;
    double sigma;

    explicit MergerState()
        : phase(MergerPhase::IDLE),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const MergerState& state) {
    std::string phaseStr = (state.phase == MergerPhase::IDLE) ? "IDLE" : "ACTIVE";
    out << "{phase=" << phaseStr << ", size=" << state.elements.size()
        << ", sigma=" << state.sigma << "}";
    return out;
}

class Merger : public Atomic<MergerState> {
public:
    Port<int> in1;  // from bay 1
    Port<int> in2;  // from bay 2
    Port<int> in3;  // from bay 3
    Port<int> in4;  // from bay 4
    Port<int> out;

    Merger(const std::string& id) : Atomic<MergerState>(id, MergerState()) {
        in1 = addInPort<int>("in1");
        in2 = addInPort<int>("in2");
        in3 = addInPort<int>("in3");
        in4 = addInPort<int>("in4");
        out = addOutPort<int>("out");
    }

    // output done check for more
    void internalTransition(MergerState& state) const override {
        if (!state.elements.empty()) {
            state.elements.pop();
        }

        if (!state.elements.empty()) {
            state.sigma = 0.0;
        } else {
            state.phase = MergerPhase::IDLE;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // planes arrive from any bay
    void externalTransition(MergerState& state, double e) const override {
        // add planes from all inputs in order
        for (const auto& plane_id : in1->getBag()) {
            state.elements.push(plane_id);
        }
        for (const auto& plane_id : in2->getBag()) {
            state.elements.push(plane_id);
        }
        for (const auto& plane_id : in3->getBag()) {
            state.elements.push(plane_id);
        }
        for (const auto& plane_id : in4->getBag()) {
            state.elements.push(plane_id);
        }

        if (!state.elements.empty()) {
            state.phase = MergerPhase::ACTIVE;
            state.sigma = 0.0;
        }
    }

    // send front plane
    void output(const MergerState& state) const override {
        if (state.phase == MergerPhase::ACTIVE && !state.elements.empty()) {
            out->addMessage(state.elements.front());
        }
    }

    [[nodiscard]] double timeAdvance(const MergerState& state) const override {
        return state.sigma;
    }
};

#endif
