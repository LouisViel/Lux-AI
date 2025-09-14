#include "CityHandler.hpp"

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

	factory->addBelief("CanSurviveNight", [this]() {
		ACCESS_CITY;
		return LuxHelper::citySurviveNight(LuxHelper::getc);
		});
	factory->addInvert("NotCanSurviveNight", "CanSurviveNight");
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
		.withStrategy(utils::make_unique<IdleStrategy>())
		.addEffect(beliefs["NoCd"])
		.withCost(1.0f)
		.buildShared()
	);
}