#pragma once
#include "../Module.hpp"
#include "../IPathfindingAlgorithm.hpp"
#include "../utils/Constraint.hpp"

class AStarPathfinder : public IPathfindingAlgorithm
{
private:
    static constexpr float actionCost = 1.0f;

private:
    struct Node
    {
    public:
        float heuristic;
        float globalCost;
        std::weak_ptr<AStarPathfinder::Node> parent;
        Position position;
        int time;
        
    public:
        Node(float heuristic, float cost, std::weak_ptr<AStarPathfinder::Node> parent, const Position& position, int time) :
            heuristic(heuristic), globalCost(cost), parent(parent), position(position), time(time)
        { }

        float getCost() const
        {
            return globalCost + heuristic;
        }
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

private:
    const int width, height;
    const Func3<bool, int, Position, int> validate;
    const Func2<float, Position, Position> heuristic;
    const Func3<float, int, Position, int> extraCost;

public:
    // Template - AStarPathfinder(int width, int height, Func<bool, int, Position> isValid);
    ~AStarPathfinder() = default;

    virtual AgentPath computePath(
        const AgentInput& agent, int time, const Constraints& constraints,
        std::weak_ptr<ReservationTable> reservationTable = std::weak_ptr<ReservationTable>()
    ) override;
    
    bool isValid(int agentId, const Position& pos, int time) const;

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

public:
    template <typename T1, typename T2, typename T3>
    struct EnableIf {
        static constexpr bool value =
            std::is_convertible<T1, Func3<bool, int, Position, int>>::value &&
            std::is_convertible<T2, Func2<float, Position, Position>>::value &&
            std::is_convertible<T2, Func3<float, int, Position, int>>::value;
    };

    template<typename T1, typename T2, typename T3, typename = typename std::enable_if<AStarPathfinder::EnableIf<T1, T2, T3>::value>::type>
    AStarPathfinder(int width, int height, T1&& validate, T2&& heuristic, T3 extraCost) :
        width(width), height(height),
        validate(std::forward<T1>(validate)),
        heuristic(std::forward<T2>(heuristic)),
        extraCost(std::forward<T3>(extraCost))
    { }
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


class AStarHelper
{
public:
    static bool validate(const int agentId, const Position& pos, int time)
    {
        return true;
    }
    
    static float manhattan(const Position& a, const Position& b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    static float noCost(const int agentId, const Position& pos, int time)
    {
        return 0.0f;
    }
};