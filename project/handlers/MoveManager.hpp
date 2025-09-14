#pragma once
#include "modules/pathfinding/MultiAgentPathfinder.hpp"
#include "modules/pathfinding/cbs/CBSPathfinder.hpp"
#include "modules/pathfinding/astar/AStarPathfinder.hpp"

class MoveManager
{
public:
	std::unique_ptr<MultiAgentPathfinder> pathfinder;
	std::unordered_set<int> agentsBuffer;

public:
	MoveManager(int width, int height);
	~MoveManager() = default;

	bool validate(int agentId, const Position& pos, int time);
	float heuristic(const Position& start, const Position& end);
	float extraCost(int agentId, const Position& pos, int time);

	bool sharedAllowed(int id1, int id2, const Position& pos, int time);

public:
	void addAgent(const std::string& agentId, const Position& start, const Position& goal, int priority = 0);
	void addAgent(int agentId, const Position& start, const Position& goal, int priority = 0);
	void removeAgent(const std::string& agentId);
	void removeAgent(int agentId);
	void reset();

	void compute(int time);
};