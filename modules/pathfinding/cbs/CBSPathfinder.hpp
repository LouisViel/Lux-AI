#pragma once
#include "../IPathFindingAlgorithm.hpp"
#include "../utils/Constraint.hpp"
#include "../utils/Conflict.hpp"

class CBSPathfinder : public IPathfindingMultiAlgorithm
{
private:
    static constexpr int MAX_NODES = 150;

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

private:
    struct Node
    {
    public:
        std::map<int, AgentPath> paths; // chemins actuels pour chaque agent
        Constraints constraints; // contraintes cumulées
        float cost = 0.0f; // coût total des chemins
        float heuristic = 0.0f; // heuristic total des chemins

    public:
        Node() = default;
        ~Node() = default;

    public:
        template <typename T1, typename T2>
        struct EnableIf {
            static constexpr bool value =
                std::is_convertible<T1, std::map<int, AgentPath>>::value&&
                std::is_convertible<T2, Constraints>::value;
        };

        template<typename T1, typename T2, typename = typename std::enable_if<CBSPathfinder::Node::EnableIf<T1, T2>::value>::type>
        Node(T1&& paths, T2&& constraints, float cost) :
            paths(std::forward<T1>(paths)),
            constraints(std::forward<T2>(constraints)),
            cost(cost)
        { }
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

private:
    std::shared_ptr<IPathfindingAlgorithm> algorithm;
    Func4<bool, int, int, Position, int> sharedAllowed;

public:
    int maxNodes = MAX_NODES;
    float heuristicAlpha = 0.0f;

public:
	// Template - CBSPathfinder(std::shared_ptr<IPathfindingAlgorithm> algorithm);

    virtual std::map<int, AgentPath> computePaths(
        const std::vector<AgentInput>& agents, int time,
        ReservationTable& reservationTable
    ) override;

private:
    std::unique_ptr<Conflict> detectConflict(const std::map<int, AgentPath>& paths) const;
    std::unique_ptr<Conflict> detectExternalConflict(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable) const;

    bool isSharedAllowed(int id1,  int id2, const Position& pos, int time) const;
    std::map<int, AgentPath> getPartialSolution(const std::vector<AgentInput>& agents, const std::map<int, AgentPath>& paths) const;

    float evaluateHeuristic(const std::shared_ptr<CBSPathfinder::Node>& node, ReservationTable& reservationTable) const;
    int countConflicts(const std::map<int, AgentPath>& paths) const;
    int countExternalConflicts(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable) const;

    float getCost(const AgentPath& path) const;
    Func1<float, float> getCostModifier(const AgentInput& agent) const;
    
    //int findOtherAgent(const Constraint& conflict, const std::map<int, AgentPath>& paths) const;
    //static std::pair<bool, Position> getPositionAtTime(const AgentPath& path, int t);

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

public:
    template <typename T1>
    struct EnableIf {
        static constexpr bool value =
            std::is_convertible<T1, Func4<bool, int, int, Position, int>>::value;
    };

    template<typename T1, typename = typename std::enable_if<CBSPathfinder::EnableIf<T1>::value>::type>
    CBSPathfinder(std::shared_ptr<IPathfindingAlgorithm> algorithm, T1&& sharedAllowed, float heuristicAlpha = 0.0f, int maxNodes = CBSPathfinder::MAX_NODES) :
        algorithm(algorithm), sharedAllowed(std::forward<T1>(sharedAllowed)), heuristicAlpha(heuristicAlpha), maxNodes(maxNodes) { }
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


class CBSHelper
{
public:
    static bool noShare(int id1, int id2, const Position& pos, int time)
    {
        return false;
    }
};