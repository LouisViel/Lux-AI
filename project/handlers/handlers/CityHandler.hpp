#pragma once
#include "GoapHandler.hpp"
#include "../../LuxHelper.hpp"

class CityHandler : public GoapHandler
{

#define ACCESS_CITY \
		const lux::CityTile* const city = LuxHelper::getCity(this->cityPosition); \
		if (!city) return false;

	int workerCreated = 0;
	int cartCreated = 0;
	lux::Position cityPosition;
	bool createdWorker;
	bool createdCart;
	bool researched;
	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;
};
