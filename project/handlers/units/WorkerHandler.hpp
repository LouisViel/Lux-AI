#pragma once
#include "../handlers/UnitHandler.hpp"

class WorkerHandler : public UnitHandler
{
private:
	bool cityBuild = false;

public:
	using UnitHandler::UnitHandler;

	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;

	bool isMining() const;
	bool isMining(const lux::ResourceType resource) const;
};