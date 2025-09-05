#pragma once
#include <map>
#include <vector>
#include "Module.hpp"

class Constraints;
class ReservationTable;


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


class IPathfindingAlgorithm
{
public:
    virtual ~IPathfindingAlgorithm() = default;
    virtual AgentPath computePath(
        const AgentInput& agent, int time, const Constraints& constraints,
        std::weak_ptr<ReservationTable> reservationTable = std::weak_ptr<ReservationTable>()
    ) = 0;

    virtual void resetCache() { }
    virtual void injectCache(AgentPath&& cache) { }
    virtual void injectCache(const AgentPath& cache)
    {
        AgentPath copy = cache; // copy
        injectCache(std::move(copy));
    }
};

class IPathfindingMultiAlgorithm
{
public:
    virtual ~IPathfindingMultiAlgorithm() = default;
    virtual std::map<int, AgentPath> computePaths(
        const std::vector<AgentInput>& agents, int time,
        ReservationTable& reservationTable
    ) = 0;
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


struct AgentInput
{
public:
    int id;
    Position start;
    Position goal;
    int priority; // optionnel

    AgentInput() = default;
    ~AgentInput() = default;
    AgentInput(int id, const Position& start, const Position& goal, int priority = 0)
        : id(id), start(start), goal(goal), priority(priority) {
    }
};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

class AgentPath : public std::vector<std::pair<Position, int>>
{
private:
    std::vector<Func1<float, float>> modifiers;
    float cost = -1.0f;

public:
    using std::vector<std::pair<Position, int>>::vector;

    void setCost(float cost)
    {
        this->cost = cost;
    }

    float evaluate(float cost) const
    {
        for (const Func1<float, float>& modifier : modifiers)
            cost = modifier(cost);
        return cost;
    }

    float getCost() const
    {
        return evaluate(cost);
    }

    bool isValid() const
    {
        return !this->empty() && cost >= 0.0f;
    }

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    template<typename T, typename = typename std::enable_if<std::is_convertible<T, Func1<float, float>>::value>::type>
    void addModifier(T&& modifier)
    {
        modifiers.push_back(modifier);
    }
};