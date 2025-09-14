#pragma once
#include "GoapHandler.hpp"
#include "../../LuxHelper.hpp"

class CityHandler : public GoapHandler
{
#define ACCESS_CITY \
		const lux::CityTile* const city = LuxHelper::getCity(this->cityPosition); \
		if (!city) return false;

public:

	int workerCreated = 0;
	int cartCreated = 0;

	lux::Position cityPosition;
	bool createdWorker;
	bool createdCart;
	bool researched;

public:
	//CityHandler() = delete;
	CityHandler(const std::string& id);

	// explicitement autoriser le move
	CityHandler(CityHandler&&) = default;
	CityHandler& operator=(CityHandler&&) = default;

	// interdire la copie
	CityHandler(const CityHandler&) = delete;
	CityHandler& operator=(const CityHandler&) = delete;

	const lux::CityTile* getTile();

protected:
	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;
};
