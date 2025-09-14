#include "CityHandler.hpp"
#include "HandlersManager.h"
#include "../strategies/CreateWorkerSrategy.h"
#include "../strategies/CreateCartStrategy.h"
#include "../strategies/ResearchStrategy.h"

CityHandler::CityHandler(const std::string& id) : GoapHandler(id) { }

void CityHandler::setupBeliefs()
{
	factory->addBelief("Nothing", []() { return false; });

	factory->addBelief("CreatedWorker", [this]() {
		return this->createdWorker;
		});
	factory->addBelief("NoCd", [this]() {
		ACCESS_CITY;
		return city->cooldown<=0;
		});
	factory->addBelief("CreatedCart", [this]() {
		return this->createdCart;
		});
	factory->addBelief("Researched", [this]() {
		return this->researched;
		});
	factory->addBelief("RatioCartWorkerNotRespected", [this]() 
		{
			ACCESS_CITY;
			const lux::City* mainCity = LuxHelper::getCity(city->cityid);
			int cityWorker = 0;
			int cityCart = 0;
			for(const lux::CityTile& tile : mainCity->citytiles)
			{
				CityHandler* cityHandler = (CityHandler*) HandlersManager::getHandler(tile.cityid);
				cityWorker += cityHandler->workerCreated;
				cityCart += cityHandler->cartCreated;
			}
			return (float)cityWorker / cityCart > 6;
		});
	factory->addBelief("FinishResearch", [this]()
		{
			return LuxHelper::canMineUranium();
		});
	factory->addInvert("NotFinishResearch", "FinishResearch");

	factory->addBelief("CanSurviveNight", [this]() {
		ACCESS_CITY;
		return LuxHelper::citySurviveNight(LuxHelper::getCity(city->cityid));
		});
	factory->addInvert("CannotSurviveNight", "CanSurviveNight");
}

void CityHandler::setupActions()
{
	ACCESS_BELIEFS;

	agent->actions->insert(AgentAction::Builder("Idle")
		.withStrategy(utils::make_unique<IdleStrategy>())
		.addEffect(beliefs["Nothing"])
		.withCost(1.0f)
		.buildShared()
	);
	agent->actions->insert(AgentAction::Builder("CreateWorker")
		.withStrategy(utils::make_unique<CreateWorkerSrategy>(this))
		.addEffect(beliefs["NoCd"])
		.withCost(1.0f)
		.buildShared()
	);
	agent->actions->insert(AgentAction::Builder("CreateCart")
		.withStrategy(utils::make_unique<CreateCartStrategy>(this))
		.addEffect(beliefs["NoCd"])
		.addEffect(beliefs["RatioCartWorkerNotRespected"])
		.withCost(1.0f)
		.buildShared()
	);
	agent->actions->insert(AgentAction::Builder("Research")
		.withStrategy(utils::make_unique<ResearchStrategy>(this))
		.addEffect(beliefs["NotFinishResearch"])
		.addEffect(beliefs["NoCd"])
		.withCost(1.0f)
		.buildShared()
	); 
	agent->actions->insert(AgentAction::Builder("InfluenceUnitForFuel")
		.withStrategy(utils::make_unique<IdleStrategy>())
		.addEffect(beliefs["CannotSurviveNight"])
		.withCost(1.0f)
		.buildShared()
	);

}

void CityHandler::setupGoals()
{
	ACCESS_BELIEFS;

	agent->goals->insert(AgentGoal::Builder("CreateMoreWorkers")
		.withPriority(1)
		.addDesiredEffect(beliefs["CreatedWorker"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("CreateMoreCarts")
		.withPriority(1)
		.addDesiredEffect(beliefs["CreatedCart"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("UnlockTechnology")
		.withPriority(1)
		.addDesiredEffect(beliefs["Researched"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("SurviveNight")
		.withPriority(1) 
		.addDesiredEffect(beliefs["CanSurviveNight"])
		.buildShared()
	);
}

const lux::CityTile* CityHandler::getTile()
{
	return LuxHelper::getCity(this->cityPosition);
}