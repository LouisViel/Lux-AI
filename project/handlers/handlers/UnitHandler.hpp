#pragma once
#include "GoapHandler.hpp"

class UnitHandler : public GoapHandler
{
protected:
	#define ACCESS_UNIT \
		const lux::Unit* const unit = LuxHelper::getUnit(this->id); \
		if (!unit) return false;

protected:
	const int nightFuel;
	const int roadFreeAction;
	bool hasPath = false;
	bool moved = false;
	bool transfered = false;
	bool destroyedRoad = false;

public:
	UnitHandler(std::string id);
	virtual ~UnitHandler() = default;

	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;
};