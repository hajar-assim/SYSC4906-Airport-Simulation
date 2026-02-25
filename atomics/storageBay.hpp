/*
 * Storage Bay Atomic Model
 *
 * Stores planes and forwards them to the merger immediately.
 * Basically a pass-through buffer - planes come in and go out right away.
 *
 * States: IDLE, ACTIVE
 *   - IDLE: No planes
 *   - ACTIVE: Outputting planes (Ta = 0)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef STORAGE_BAY_HPP
#define STORAGE_BAY_HPP

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <queue>
#include <limits>

using namespace cadmium;

enum class StorageBayPhase { IDLE, ACTIVE };

struct StorageBayState {
    StorageBayPhase phase;
    std::queue<int> elements;
    double sigma;

    explicit StorageBayState()
        : phase(StorageBayPhase::IDLE),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const StorageBayState& state) {
    std::string phaseStr = (state.phase == StorageBayPhase::IDLE) ? "IDLE" : "ACTIVE";
    out << "{phase=" << phaseStr << ", size=" << state.elements.size()
        << ", sigma=" << state.sigma << "}";
    return out;
}

class StorageBay : public Atomic<StorageBayState> {
public:
    Port<int> in;
    Port<int> out;

    StorageBay(const std::string& id) : Atomic<StorageBayState>(id, StorageBayState()) {
        in = addInPort<int>("in");
        out = addOutPort<int>("out");
    }

    // output done check for more
    void internalTransition(StorageBayState& state) const override {
        if (!state.elements.empty()) {
            state.elements.pop();
        }

        if (!state.elements.empty()) {
            // more planes waiting
            state.sigma = 0.0;
        } else {
            // empty go idle
            state.phase = StorageBayPhase::IDLE;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // new planes arrive
    void externalTransition(StorageBayState& state, double e) const override {
        for (const auto& plane_id : in->getBag()) {
            state.elements.push(plane_id);
        }

        if (!state.elements.empty()) {
            state.phase = StorageBayPhase::ACTIVE;
            state.sigma = 0.0;
        }
    }

    // send front plane
    void output(const StorageBayState& state) const override {
        if (state.phase == StorageBayPhase::ACTIVE && !state.elements.empty()) {
            out->addMessage(state.elements.front());
        }
    }

    [[nodiscard]] double timeAdvance(const StorageBayState& state) const override {
        return state.sigma;
    }
};

#endif
