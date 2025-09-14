#pragma once
#include "modules/goap/IActionStrategy.hpp"
class CityHandler;

class CreateCartStrategy : public IActionStrategy
{
	public:
		CreateCartStrategy(CityHandler* cityHandler);
		CityHandler* handler;
		virtual ~CreateCartStrategy() = default;

		virtual bool canPerform() const override;
		virtual bool isComplete() const override;

		virtual void start() override;
		virtual void update(int turnId) override;
		virtual void stop() override;
};