#pragma once
#include "../PathFindingAlgorithm.hpp"
#include "../utils/PathCache.hpp"
#include "../utils/Constraint.hpp"
#include "../utils/Conflict.hpp"

class CBSPathfinder : public IPathfindingMultiAlgorithm
{
private:
    static constexpr int MAX_NODES = 400;

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

private:
    struct Node
    {
    public:
        std::map<int, PathPtr> paths; // chemins actuels pour chaque agent
        Constraints constraints; // contraintes cumulées
        float cost = 0.0f; // coût total des chemins
        float heuristic = 0.0f; // heuristic total des chemins

        // Cache des conflits connus
        int conflictCount = -1; // -1 => non calculé
        ConflictMap conflicts; // conflict by agent group
        std::unique_ptr<Conflict> lastConflict; // dernier conflit mis en cache

    public:
        ~Node() = default;
        Node() = default;
        Node(const CBSPathfinder::Node& other) :
            paths(other.paths),
            constraints(other.constraints),
            cost(other.cost),
            heuristic(other.heuristic),
            conflictCount(other.conflictCount),
            conflicts(other.conflicts),
            lastConflict(nullptr)
        { }

        std::map<int, AgentPath> exportPaths(PathCache* const cache = nullptr)
        {
            std::map<int, AgentPath> paths;
            for (std::map<int, PathPtr>::const_iterator it = this->paths.begin(); it != this->paths.end(); ++it)
                paths.emplace(it->first, std::move(*it->second));
            if (cache) cache->reset();
            return paths;
        }

    public:
        template <typename T1, typename T2>
        struct EnableIf {
            static constexpr bool value =
                std::is_convertible<T1, std::map<int, PathPtr>>::value&&
                std::is_convertible<T2, Constraints>::value;
        };

        template<typename T1, typename T2, typename = typename std::enable_if<CBSPathfinder::Node::EnableIf<T1, T2>::value>::type>
        Node(T1&& paths, T2&& constraints, float cost) :
            paths(std::forward<T1>(paths)),
            constraints(std::forward<T2>(constraints)),
            cost(cost)
        { }
    };

    typedef std::shared_ptr<CBSPathfinder::Node> NodePtr;

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

private:
    const Func4<bool, int, int, Position, int> sharedAllowed;
    PathCache pathCache;

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
    bool isSharedAllowed(int id1,  int id2, const Position& pos, int time) const;
    std::map<int, AgentPath> getPartialSolution(const std::vector<AgentInput>& agents, int time, const NodePtr& node);
    float getCost(const PathPtr& path) const;
    Func1<float, float> getCostModifier(const AgentInput& agent) const;

    float evaluateHeuristic(const NodePtr& node, ReservationTable& reservationTable) const;
    void updateConflicts(const NodePtr& node, int changedAgent, ReservationTable& reservationTable) const;
    void heuristicFallback(const NodePtr& node, ReservationTable& reservationTable) const;

    std::unique_ptr<Conflict> detectConflict(const std::map<int, PathPtr>& paths) const;
    int countConflictAndCache(const NodePtr& node) const;
    int countConflictPairAndCache(const NodePtr& node, const PathPtr& p1, const PathPtr& p2, int id1, int id2) const;
    void forEachConflict(const PathPtr& p1Ptr, const PathPtr& p2Ptr, int id1, int id2, const Func4<bool, int, int, Position, int>& callback) const;

    std::unique_ptr<Conflict> detectEConflict(const std::map<int, PathPtr>& paths, ReservationTable& reservationTable) const;
    int countEConflictAndCache(const NodePtr& node, ReservationTable& reservationTable) const;
    int countEConflictAgentAndCache(const NodePtr& node, const PathPtr& path, int id, ReservationTable& reservationTable) const;
    void forEachEConflict(const PathPtr& path, int id, ReservationTable& rt, const Func4<bool, int, int, Position, int>& callback) const;

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
        pathCache(algorithm), sharedAllowed(std::forward<T1>(sharedAllowed)), heuristicAlpha(heuristicAlpha), maxNodes(maxNodes) { }
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