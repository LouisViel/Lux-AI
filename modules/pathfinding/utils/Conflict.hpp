#pragma once
#include "../Module.hpp"

struct Conflict
{
    int agentA;
    int agentB;
    Position pos;
    int time;

    Conflict() = default;
    Conflict(int agentA, int agentB, const Position& pos, int time) :
        agentA(agentA), agentB(agentB), pos(pos), time(time) { }
    ~Conflict() = default;
};