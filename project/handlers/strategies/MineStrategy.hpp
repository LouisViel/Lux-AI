#pragma once
#include "modules/goap/IActionStrategy.hpp"
#include "modules/pathfinding/PathFindingAlgorithm.hpp"

class WorkerHandler;

class MineStrategy : public IActionStrategy
{
private:
	WorkerHandler* worker;
	bool finished = false;
	bool readyToMine = false;
	bool minePathSkip = false;
	
	AgentPath minePath;
	int pathId = 0;

public:
	MineStrategy(WorkerHandler* worker);
	virtual ~MineStrategy() = default;

	virtual bool canPerform() const override;
	virtual bool isComplete() const override;

	virtual void start() override;
	virtual void update(int turnId) override;
	virtual void stop() override;
};