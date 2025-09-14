#pragma once
#include "modules/goap/IActionStrategy.hpp"
class CityHandler;

class CreateWorkerSrategy : public IActionStrategy
{
public :
	CreateWorkerSrategy(CityHandler* cityHandler);
	CityHandler* handler;
	virtual ~CreateWorkerSrategy() = default;

	virtual bool canPerform() const override;
	virtual bool isComplete() const override;

	virtual void start() override;
	virtual void update(int turnId) override;
	virtual void stop() override;
};

