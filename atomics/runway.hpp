/*
 * Runway Atomic Model
 *
 * Single runway for both landing and takeoff. Takes 60 seconds per operation.
 *
 * States: IDLE, LANDING, TAKEOFF
 *   - IDLE: Runway is free
 *   - LANDING: Plane is landing (60s)
 *   - TAKEOFF: Plane is taking off (60s)
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef RUNWAY_HPP
#define RUNWAY_HPP

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <limits>

using namespace cadmium;

enum class RunwayPhase { IDLE, LANDING, TAKEOFF };

struct RunwayState {
    RunwayPhase phase;
    int plane_id;
    double sigma;

    static constexpr double RUNWAY_TIME = 60.0;  // 1 min

    explicit RunwayState()
        : phase(RunwayPhase::IDLE),
          plane_id(0),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const RunwayState& state) {
    std::string phaseStr;
    switch (state.phase) {
        case RunwayPhase::IDLE:    phaseStr = "IDLE"; break;
        case RunwayPhase::LANDING: phaseStr = "LANDING"; break;
        case RunwayPhase::TAKEOFF: phaseStr = "TAKEOFF"; break;
    }
    out << "{phase=" << phaseStr << ", plane=" << state.plane_id
        << ", sigma=" << state.sigma << "}";
    return out;
}

class Runway : public Atomic<RunwayState> {
public:
    Port<int> land;
    Port<int> takeoff;
    Port<int> landing_exit;
    Port<int> takeoff_exit;

    Runway(const std::string& id) : Atomic<RunwayState>(id, RunwayState()) {
        land = addInPort<int>("land");
        takeoff = addInPort<int>("takeoff");
        landing_exit = addOutPort<int>("landing_exit");
        takeoff_exit = addOutPort<int>("takeoff_exit");
    }

    // go back to idle after operation
    void internalTransition(RunwayState& state) const override {
        state.phase = RunwayPhase::IDLE;
        state.sigma = std::numeric_limits<double>::infinity();
    }

    // start landing or takeoff
    void externalTransition(RunwayState& state, double e) const override {
        if (!land->empty()) {
            state.plane_id = land->getBag().back();
            state.phase = RunwayPhase::LANDING;
            state.sigma = RunwayState::RUNWAY_TIME;
        }
        else if (!takeoff->empty()) {
            state.plane_id = takeoff->getBag().back();
            state.phase = RunwayPhase::TAKEOFF;
            state.sigma = RunwayState::RUNWAY_TIME;
        }
    }

    // send plane to appropriate exit
    void output(const RunwayState& state) const override {
        switch (state.phase) {
            case RunwayPhase::LANDING:
                landing_exit->addMessage(state.plane_id);
                break;
            case RunwayPhase::TAKEOFF:
                takeoff_exit->addMessage(state.plane_id);
                break;
            case RunwayPhase::IDLE:
                break;
        }
    }

    [[nodiscard]] double timeAdvance(const RunwayState& state) const override {
        return state.sigma;
    }
};

#endif
