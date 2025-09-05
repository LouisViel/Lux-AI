#pragma once
#include "../Module.hpp"

struct Constraint
{
    int agentId;
    Position pos;
    int time;

    Constraint() = default;
    Constraint(int agentId, const Position& pos, int time) : agentId(agentId), pos(pos), time(time) { }
    ~Constraint() = default;
};

class Constraints : public std::vector<Constraint>
{
public:
    using std::vector<Constraint>::vector;

    bool has(int agentId, const Position& pos, int t) const
    {
        for (const Constraint& constraint : *this) {
            if (constraint.agentId == agentId &&
                constraint.pos == pos &&
                constraint.time == t
            ) return true;
        } return false;
    }

    void add(int agentId, const Position& pos, int t)
    {
        this->push_back(Constraint(agentId, pos, t));
    }

    void remove(int agentId, const Position& pos, int t)
    {
        for (std::vector<Constraint>::const_iterator it = this->begin(); it != this->end(); ++it) {
            if (it->agentId == agentId && it->pos == pos && it->time == t) {
                this->erase(it);
                break;
            }
        }
    }
};