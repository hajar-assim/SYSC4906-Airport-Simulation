/*
 * Hangar Coupled Model
 *
 * Routes incoming planes through selector to storage bank.
 * Selector assigns planes to bays based on ID ranges,
 * storage bank holds planes and drains them to the exit.
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#ifndef HANGAR_HPP
#define HANGAR_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/selector.hpp"
#include "storageBank.hpp"

using namespace cadmium;

class Hangar : public Coupled {
public:
    Port<int> in;
    Port<int> hangar_exit;

    Hangar(const std::string& id) : Coupled(id) {
        in = addInPort<int>("in");
        hangar_exit = addOutPort<int>("hangar_exit");

        auto selector = addComponent<Selector>("Selector");
        auto storageBank = addComponent<StorageBank>("StorageBank");

        addCoupling(in, selector->in);

        addCoupling(selector->out1, storageBank->in1);
        addCoupling(selector->out2, storageBank->in2);
        addCoupling(selector->out3, storageBank->in3);
        addCoupling(selector->out4, storageBank->in4);

        addCoupling(storageBank->out, hangar_exit);
    }
};

#endif // HANGAR_HPP
