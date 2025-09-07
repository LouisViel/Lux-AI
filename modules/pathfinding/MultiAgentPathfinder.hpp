#pragma once
#include "Module.hpp"
#include "PathFindingAlgorithm.hpp"

class ReservationTable;

class MultiAgentPathfinder
{
private:
    struct Agent
    {
    public:
        Position start;
        Position goal;
        AgentPath path;
        int priority = 0;

    public:
        Agent() = default;
        ~Agent() = default;

        template <typename T, typename = typename std::enable_if<std::is_convertible<T, AgentPath>::value>::type>
        Agent(const Position& start, const Position& goal, T&& path, int priority = 0)
            : start(start), goal(goal), path(std::forward<T>(path)), priority(priority) { }
    };

private:
    std::unique_ptr<ReservationTable> reservationTable;
    std::map<int, Agent> agents;
    std::shared_ptr<IPathfindingMultiAlgorithm> algorithm;

public:
    MultiAgentPathfinder(int width, int height, std::shared_ptr<IPathfindingMultiAlgorithm> algorithm);
    ~MultiAgentPathfinder();

    void addStay(int agentId, const Position& start);
    void addAgent(int agentId, const Position& start, const Position& goal, int priority = 0);
    void removeAgent(int agentId);
    void clear();

    AgentPath getPath(int agentId) const;
    bool hasPath(int agentId) const;

    void computePaths(int time = 0);
    void computePath(int agentId, int time = 0);
    void computePath(std::vector<int> agentIds, int time = 0);

    void resetPaths();
    void resetPath(int agentId);
    void resetPath(std::vector<int> agentIds);

private:
    void computePathInternal(const std::vector<AgentInput>& inputs, int time);
};