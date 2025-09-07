#include "UnitHandler.hpp"

UnitHandler::UnitHandler() : GoapHandler() { }

void UnitHandler::setupBeliefs()
{
	factory->addBelief("Nothing", []() { return false; });
	factory->addBelief("Idle", []() { return false; });
}

void UnitHandler::setupActions()
{
	ACCESS_BELIEFS
	agent->actions->insert(AgentAction::Builder("Move")
		.withStrategy(nullptr)
		.addEffect(beliefs["Nothing"])
		.addPrecondition(beliefs["Nothing"])
		.withCost(1.0f)
		.buildShared()
	);
}

void UnitHandler::setupGoals()
{
	ACCESS_BELIEFS
	agent->goals->insert(AgentGoal::Builder("Survive")
		.withPriority(1)
		.withDesiredEffect(beliefs["Nothing"])
		.buildShared()
	);
}