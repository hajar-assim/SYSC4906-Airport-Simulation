/*
 * Control Tower Atomic Model
 *
 * Manages runway access - makes sure only one plane uses the runway at a time.
 * Prioritizes landings over takeoffs for safety.
 *
 * States: IDLE -> SIGNAL -> WAIT -> IDLE
 *   - IDLE: Waiting for a plane request
 *   - SIGNAL: Send stop signals to queues and command to runway (Ta = 0)
 *   - WAIT: Runway is busy, wait 60 seconds
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef CONTROL_TOWER_HPP
#define CONTROL_TOWER_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>

using namespace cadmium;

enum class TowerPhase { IDLE, SIGNAL, WAIT };
enum class OperationType { NONE, LANDING, TAKEOFF };

struct ControlTowerState {
    TowerPhase phase;
    OperationType operation_type;
    int plane_id;
    double sigma;

    static constexpr double RUNWAY_TIME = 60.0;  // 1 min for runway ops

    explicit ControlTowerState()
        : phase(TowerPhase::IDLE),
          operation_type(OperationType::NONE),
          plane_id(0),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const ControlTowerState& state) {
    std::string phaseStr;
    switch (state.phase) {
        case TowerPhase::IDLE:   phaseStr = "IDLE"; break;
        case TowerPhase::SIGNAL: phaseStr = "SIGNAL"; break;
        case TowerPhase::WAIT:   phaseStr = "WAIT"; break;
    }
    std::string opStr;
    switch (state.operation_type) {
        case OperationType::NONE:    opStr = "NONE"; break;
        case OperationType::LANDING: opStr = "LANDING"; break;
        case OperationType::TAKEOFF: opStr = "TAKEOFF"; break;
    }
    out << "{phase=" << phaseStr << ", op=" << opStr
        << ", plane=" << state.plane_id << ", sigma=" << state.sigma << "}";
    return out;
}

class ControlTower : public Atomic<ControlTowerState> {
public:
    // inputs
    Port<int> in_landing;
    Port<int> in_takeoff;

    // outputs
    Port<int> stop_landing;
    Port<int> stop_takeoff;
    Port<int> done_landing;
    Port<int> done_takeoff;
    Port<int> land;
    Port<int> takeoff;

    ControlTower(const std::string& id) : Atomic<ControlTowerState>(id, ControlTowerState()) {
        in_landing = addInPort<int>("in_landing");
        in_takeoff = addInPort<int>("in_takeoff");

        stop_landing = addOutPort<int>("stop_landing");
        stop_takeoff = addOutPort<int>("stop_takeoff");
        done_landing = addOutPort<int>("done_landing");
        done_takeoff = addOutPort<int>("done_takeoff");
        land = addOutPort<int>("land");
        takeoff = addOutPort<int>("takeoff");
    }

    void internalTransition(ControlTowerState& state) const override {
        switch (state.phase) {
            case TowerPhase::SIGNAL:
                // just sent commands now wait for runway
                state.phase = TowerPhase::WAIT;
                state.sigma = ControlTowerState::RUNWAY_TIME;
                break;

            case TowerPhase::WAIT:
                // runway done go back to idle
                state.phase = TowerPhase::IDLE;
                state.operation_type = OperationType::NONE;
                state.sigma = std::numeric_limits<double>::infinity();
                break;

            case TowerPhase::IDLE:
                state.sigma = std::numeric_limits<double>::infinity();
                break;
        }
    }

    void externalTransition(ControlTowerState& state, double e) const override {
        // only accept requests when idle
        if (state.phase != TowerPhase::IDLE) return;

        // landing has priority
        if (!in_landing->empty()) {
            state.plane_id = in_landing->getBag().back();
            state.operation_type = OperationType::LANDING;
            state.phase = TowerPhase::SIGNAL;
            state.sigma = 0.0;
        }
        else if (!in_takeoff->empty()) {
            state.plane_id = in_takeoff->getBag().back();
            state.operation_type = OperationType::TAKEOFF;
            state.phase = TowerPhase::SIGNAL;
            state.sigma = 0.0;
        }
    }

    void output(const ControlTowerState& state) const override {
        switch (state.phase) {
            case TowerPhase::SIGNAL:
                // stop both queues
                stop_landing->addMessage(1);
                stop_takeoff->addMessage(1);

                // send runway command
                if (state.operation_type == OperationType::LANDING) {
                    land->addMessage(state.plane_id);
                } else if (state.operation_type == OperationType::TAKEOFF) {
                    takeoff->addMessage(state.plane_id);
                }
                break;

            case TowerPhase::WAIT:
                // tell queues they can resume
                done_landing->addMessage(1);
                done_takeoff->addMessage(1);
                break;

            case TowerPhase::IDLE:
                break;
        }
    }

    [[nodiscard]] double timeAdvance(const ControlTowerState& state) const override {
        return state.sigma;
    }
};

#endif
