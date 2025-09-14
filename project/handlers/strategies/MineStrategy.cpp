#include "MineStrategy.hpp"
#include "../../LuxHelper.hpp"
#include "../units/WorkerHandler.hpp"
#include "../MoveManager.hpp"
#include "../MineManager.hpp"

MineStrategy::MineStrategy(WorkerHandler* worker) : worker(worker) { }

bool MineStrategy::canPerform() const
{
	return worker->isMining() || worker->canMove();
}

bool MineStrategy::isComplete() const
{
	return this->finished;
}

void MineStrategy::start()
{
	// Check if ready to mine already
	if (worker->isMining()) {
		readyToMine = true;
		return;
	}

	// Declare access
	ACCESS_MOVE;
	ACCESS_MINE;
	
	// Prepare settings
	const std::string id = worker->getId();
	int agentId = LuxHelper::getId(id);
	const lux::Unit* const unit = LuxHelper::getUnit(id);

	// Get a target position & ask for a path to it
	const lux::Position targetPos = mineManager.getTarget(id, unit->pos, true, true, true);
	if (targetPos.x >= 0) moveManager.addAgent(agentId, unit->pos, targetPos, 0);
}

void MineStrategy::update(int turnId)
{
	if (finished) return;
	if (!readyToMine) {
		if (!minePathSkip) {
			minePathSkip = true;
			return;
		}

		ACCESS_MOVE;
		int agentId = LuxHelper::getId(worker->getId());
		minePath = moveManager.pathfinder->getPath(agentId);
		if (!minePath.isValid() && !worker->isMining()) {
			this->stop();
			return;
		} else readyToMine = true;
	}

	// Move if still a path to move to mine
	if (minePath.isValid()) {
		const lux::Unit* const unit = LuxHelper::getUnit(worker->getId());
		if (!unit || pathId >= minePath.size()) {
			this->stop();
			return;
		}
		
		// Check if path calculated move turn still correct
		const std::pair<Position, int>& path = minePath.at(pathId);
		if (path.second < turnId) {
			this->stop();
			return;
		}
		
		// If all correct, register move action
		if (path.second == turnId) {
			ACCESS_OUTPUT;
			const std::string action = unit->move(LuxHelper::getDirection(unit->pos, path.first));
			luxOutput.push_back(action);
		
			// Check if can end moving and just mine
			if (++pathId >= minePath.size()) {
				minePath = AgentPath::invalid(path.first, turnId);
			}
		}

	// If not moving, check if mining at least (otherwise somehting went wrong)
	} else if (!worker->isMining()) {
		this->stop();
		return;
	}

	// Stop mining when full inventory
	if (worker->isFullInventory()) {
		this->stop();
		return;
	}
}

void MineStrategy::stop()
{
	if (this->finished) return;
	this->finished = true;

	ACCESS_MOVE;
	ACCESS_MINE;
	const std::string& id = worker->getId();
	int agentId = LuxHelper::getId(id);
	moveManager.removeAgent(agentId);
	mineManager.remove(id);
}