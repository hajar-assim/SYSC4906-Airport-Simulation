/*
 * Queue Atomic Model
 *
 * FIFO buffer for plane requests. Used for both landing and takeoff queues.
 * Tower can pause the queue with stop signal, resume with done signal.
 *
 * States: IDLE, SENDING, WAIT_ACK
 *   - IDLE: Nothing to send or paused
 *   - SENDING: About to output front plane (Ta = 0)
 *   - WAIT_ACK: Sent plane, waiting for tower to finish
 *
 * Authors: Hasib Khodayar & Hajar Assim
 */

#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <queue>
#include <limits>

using namespace cadmium;

enum class QueuePhase { IDLE, SENDING, WAIT_ACK };

struct QueueState {
    QueuePhase phase;
    std::queue<int> elements;
    bool busy;  // true when tower said stop
    double sigma;

    explicit QueueState()
        : phase(QueuePhase::IDLE),
          busy(false),
          sigma(std::numeric_limits<double>::infinity()) {}
};

std::ostream& operator<<(std::ostream &out, const QueueState& state) {
    std::string phaseStr;
    switch (state.phase) {
        case QueuePhase::IDLE:     phaseStr = "IDLE"; break;
        case QueuePhase::SENDING:  phaseStr = "SENDING"; break;
        case QueuePhase::WAIT_ACK: phaseStr = "WAIT_ACK"; break;
    }
    out << "{phase=" << phaseStr << ", size=" << state.elements.size()
        << ", busy=" << state.busy << ", sigma=" << state.sigma << "}";
    return out;
}

class Queue : public Atomic<QueueState> {
public:
    Port<int> in;
    Port<int> stop;
    Port<int> done;
    Port<int> out;

    Queue(const std::string& id) : Atomic<QueueState>(id, QueueState()) {
        in = addInPort<int>("in");
        stop = addInPort<int>("stop");
        done = addInPort<int>("done");
        out = addOutPort<int>("out");
    }

    // after sending wait for ack
    void internalTransition(QueueState& state) const override {
        if (state.phase == QueuePhase::SENDING) {
            if (!state.elements.empty()) {
                state.elements.pop();
            }
            state.phase = QueuePhase::WAIT_ACK;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // handle stop done and new planes
    void externalTransition(QueueState& state, double e) const override {
        // stop signal from tower
        if (!stop->empty()) {
            state.busy = true;
            if (state.phase == QueuePhase::SENDING) {
                state.phase = QueuePhase::IDLE;
            }
            state.sigma = std::numeric_limits<double>::infinity();
        }

        // done signal tower finished we can resume
        if (!done->empty()) {
            state.busy = false;

            if (state.phase == QueuePhase::WAIT_ACK) {
                state.phase = QueuePhase::IDLE;
            }

            // if we have more planes send next one
            if (!state.elements.empty() && state.phase == QueuePhase::IDLE) {
                state.phase = QueuePhase::SENDING;
                state.sigma = 0.0;
            }
        }

        // new planes coming in always add them
        for (const auto& plane_id : in->getBag()) {
            state.elements.push(plane_id);
        }

        // if we can send and have planes do it
        if (!state.elements.empty() &&
            state.phase == QueuePhase::IDLE &&
            !state.busy) {
            state.phase = QueuePhase::SENDING;
            state.sigma = 0.0;
        }
    }

    // send front plane
    void output(const QueueState& state) const override {
        if (state.phase == QueuePhase::SENDING && !state.elements.empty()) {
            out->addMessage(state.elements.front());
        }
    }

    [[nodiscard]] double timeAdvance(const QueueState& state) const override {
        return state.sigma;
    }
};

#endif
