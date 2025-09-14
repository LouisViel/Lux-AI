#pragma once
#include "modules/goap/IActionStrategy.hpp"

class WorkerHandler;

class MineStrategy : public IActionStrategy
{
private:
	WorkerHandler* worker;

public:
	MineStrategy(WorkerHandler* worker);
	virtual ~MineStrategy() = default;

	virtual bool canPerform() const override;
	virtual bool isComplete() const override;

	virtual void start() override;
	virtual void update(int turnId) override;
	virtual void stop() const override;
};