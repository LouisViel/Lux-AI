#pragma once
#include "modules/goap/IActionStrategy.hpp"

class IdleStrategy : public IActionStrategy
{
public:
	virtual ~IdleStrategy() = default;

	virtual bool canPerform() const override;
	virtual bool isComplete() const override;

	virtual void start() override;
	virtual void update(int turnId) override;
	virtual void stop() override;
};