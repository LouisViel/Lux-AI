#pragma once
#include "../Module.hpp"

struct Conflict
{
public:
    int agentA;
    int agentB;
    Position pos;
    int time;

    Conflict() = default;
    Conflict(int agentA, int agentB, const Position& pos, int time) :
        agentA(agentA), agentB(agentB), pos(pos), time(time) { }
    ~Conflict() = default;
};

struct ConflictCore
{
public:
    int agentA;
    int agentB;

    ConflictCore() = default;
    ~ConflictCore() = default;

    ConflictCore(const Conflict& conflict) : ConflictCore(conflict.agentA, conflict.agentB) { }
    ConflictCore(int agentA, int agentB)
    {
        if (agentA < agentB) { this->agentA = agentA; this->agentB = agentB; }
        else { this->agentA = agentB; this->agentB = agentA; }
    }

    bool operator==(const ConflictCore& other) const
    {
        return agentA == other.agentA && agentB == other.agentB;
    }

    inline bool has(int agentId) const
    {
        return agentA == agentId || agentB == agentId;
    }
};

struct ConflictCoreHash {
    std::size_t operator()(const ConflictCore& c) const {
        return std::hash<int>()(c.agentA) ^ (std::hash<int>()(c.agentB) << 1);
    }
};

// Storage: each pair once, with a counter
class ConflictMap : public std::unordered_map<ConflictCore, int, ConflictCoreHash>
{
public:
    using std::unordered_map<ConflictCore, int, ConflictCoreHash>::unordered_map;

    int getAndRemove(int agentId)
    {
        int sum = 0;
        for (auto it = this->begin(); it != this->end(); /* nothing */) {
            if (it->first.has(agentId)) {
                sum += it->second;
                it = this->erase(it);
            } else ++it;
        } return sum;
    }
};