#include "MoveManager.hpp"
#include "../LuxHelper.hpp"

MoveManager::MoveManager(int width, int height)
{
	std::shared_ptr<IPathfindingAlgorithm> astar = std::make_shared<AStarPathfinder>(width, height,
		Func3<bool, int, Position, int>([this](int agentId, const Position& pos, int time) { return this->validate(agentId, pos, time); }),
		Func2<float, Position, Position>([this](const Position& start, const Position& end) { return this->heuristic(start, end); }),
		Func3<bool, int, Position, int>([this](int agentId, const Position& pos, int time) { return this->extraCost(agentId, pos, time); })
	);

	std::shared_ptr<IPathfindingMultiAlgorithm> cbs = std::make_shared<CBSPathfinder>(astar,
		Func4<bool, int, int, Position, int>([this](int id1, int id2, const Position& pos, int time) { return this->sharedAllowed(id1, id2, pos, time); }),
		0.0f, // Heuristic Alpha
		400 // max Nodes
	);

	pathfinder = utils::make_unique<MultiAgentPathfinder>(width, height, cbs);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void MoveManager::addAgent(const std::string& agentId, const Position& start, const Position& goal, int priority)
{
	this->addAgent(LuxHelper::getId(agentId), start, goal, priority);
}

void MoveManager::addAgent(int agentId, const Position& start, const Position& goal, int priority)
{
	agentsBuffer.insert(agentId);
	pathfinder->addAgent(agentId, start, goal, priority);
}

void MoveManager::removeAgent(const std::string& agentId)
{
	this->removeAgent(LuxHelper::getId(agentId));
}

void MoveManager::removeAgent(int agentId)
{
	agentsBuffer.erase(agentId);
	pathfinder->removeAgent(agentId);
}

void MoveManager::reset()
{
	agentsBuffer.clear();
	pathfinder->clear();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void MoveManager::compute(int time)
{
	if (agentsBuffer.size() <= 0) return;
	std::vector<int> buffer(agentsBuffer.begin(), agentsBuffer.end());
	pathfinder->computePath(buffer, time);
	agentsBuffer.clear();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool MoveManager::validate(int agentId, const Position& pos, int time)
{
	// No ennemy city
	const lux::Cell* const cell = LuxHelper::getCell(pos);
	const lux::Unit* const unit = LuxHelper::getUnit(LuxHelper::getId(agentId));
	if (!cell || !unit) return false;
	return cell->citytile ? cell->citytile->team == unit->team : true;
}

float MoveManager::heuristic(const Position& start, const Position& end)
{
	return AStarHelper::manhattan(start, end);;
}

float MoveManager::extraCost(int agentId, const Position& pos, int time)
{
	return 0.0f; // TODO : Implement
}

bool MoveManager::sharedAllowed(int id1, int id2, const Position& pos, int time)
{
	const lux::Cell* const cell = LuxHelper::getCell(pos);
	const lux::Unit* const unit = LuxHelper::getUnit(LuxHelper::getId(id1));
	if (!cell || !unit) return false;
	return cell->citytile ? cell->citytile->team == unit->team : false;
}