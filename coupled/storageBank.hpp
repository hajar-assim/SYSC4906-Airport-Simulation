/*
 * StorageBank Coupled Model
 *
 * Contains 4 storage bays and a merger. Each bay receives planes
 * from a specific selector output and drains to the merger.
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#ifndef STORAGE_BANK_HPP
#define STORAGE_BANK_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/storageBay.hpp"
#include "../atomics/merger.hpp"

using namespace cadmium;

class StorageBank : public Coupled {
public:
    Port<int> in1, in2, in3, in4;
    Port<int> out;

    StorageBank(const std::string& id) : Coupled(id) {
        in1 = addInPort<int>("in1");
        in2 = addInPort<int>("in2");
        in3 = addInPort<int>("in3");
        in4 = addInPort<int>("in4");
        out = addOutPort<int>("out");

        auto bay1 = addComponent<StorageBay>("Bay1");
        auto bay2 = addComponent<StorageBay>("Bay2");
        auto bay3 = addComponent<StorageBay>("Bay3");
        auto bay4 = addComponent<StorageBay>("Bay4");
        auto merger = addComponent<Merger>("Merger");

        // external input couplings
        addCoupling(in1, bay1->in);
        addCoupling(in2, bay2->in);
        addCoupling(in3, bay3->in);
        addCoupling(in4, bay4->in);

        // internal couplings bays to merger
        addCoupling(bay1->out, merger->in1);
        addCoupling(bay2->out, merger->in2);
        addCoupling(bay3->out, merger->in3);
        addCoupling(bay4->out, merger->in4);

        // external output coupling
        addCoupling(merger->out, out);
    }
};

#endif // STORAGE_BANK_HPP
