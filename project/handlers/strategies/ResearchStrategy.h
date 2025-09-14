#pragma once
#include "modules/goap/IActionStrategy.hpp"
class CityHandler;

class ResearchStrategy : public IActionStrategy
{
	public :
		ResearchStrategy(CityHandler* cityHandler);
		CityHandler* handler;
		virtual ~ResearchStrategy() = default;

		virtual bool canPerform() const override;
		virtual bool isComplete() const override;

		virtual void start() override;
		virtual void update(int turnId) override;
		virtual void stop() override;
};