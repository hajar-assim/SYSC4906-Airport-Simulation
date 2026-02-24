/**
 * Plane Message Data Structure
 *
 * Simple wrapper for plane ID used in airport simulation.
 * We use int directly for plane IDs, but this file exists for
 * consistency with the AlternatingBitProtocol repo structure.
 *
 * Plane IDs range from 0-999 and determine storage bay assignment:
 *   - Bay 1: IDs 0-249
 *   - Bay 2: IDs 250-499
 *   - Bay 3: IDs 500-749
 *   - Bay 4: IDs 750-999
 *
 * Authors: Hasib Khodayar & Hajar Assim
 * Course: SYSC 4906G, Winter 2026
 */

#ifndef PLANE_MESSAGE_HPP
#define PLANE_MESSAGE_HPP

#include <iostream>

/**
 * PlaneMessage - Optional wrapper for plane ID
 *
 * The simulation primarily uses int directly for plane IDs.
 * This struct is provided for future extensibility if additional
 * metadata (fuel level, priority, etc.) needs to be added.
 */
struct PlaneMessage {
    int plane_id;

    PlaneMessage() : plane_id(0) {}
    PlaneMessage(int id) : plane_id(id) {}

    // conversion to from int for compatibility
    operator int() const { return plane_id; }
};

std::ostream& operator<<(std::ostream& out, const PlaneMessage& msg) {
    out << msg.plane_id;
    return out;
}

std::istream& operator>>(std::istream& in, PlaneMessage& msg) {
    in >> msg.plane_id;
    return in;
}

#endif // PLANE_MESSAGE_HPP
