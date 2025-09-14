#pragma once
#include "utils.hpp"

#include "lux/position.hpp"
using Position = lux::Position;

//struct PositionHash {
//    std::size_t operator()(const Position& pos) const
//    {
//        // Combinaison simple des coordonnées
//        return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
//    }
//};

struct PosTime {
    const Position pos;
    const int time;

    PosTime(const Position& pos, int time) :
        pos(pos), time(time) { }

    bool operator==(const PosTime& other) const
    {
        return pos == other.pos && time == other.time;
    }
};

struct PosTimeHash {
    std::size_t operator()(const PosTime& pt) const
    {
        return PositionHash()(pt.pos) ^ (std::hash<int>()(pt.time) << 1);
    }
};