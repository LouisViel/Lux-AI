#include "CartHandler.hpp"

void CartHandler::setupBeliefs()
{
	UnitHandler::setupBeliefs();
	ACCESS_BELIEFS;
}

void CartHandler::setupActions()
{
	UnitHandler::setupActions();
	ACCESS_BELIEFS;


}

void CartHandler::setupGoals()
{
	UnitHandler::setupGoals();
	ACCESS_BELIEFS;

	agent->goals->insert(AgentGoal::Builder("UpgradeRoads")
		.withPriority(1) // TODO : Implement
		.addDesiredEffect(beliefs["RoadUpgraded"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("TransportResources")
		.withPriority(1) // TODO : Implement
		.addDesiredEffect(beliefs["InventoryFilled"])
		.buildShared()
	);

	//agent->goals->insert(AgentGoal::Builder("HelperWorker")
	//	.withPriority(1) // TODO : Implement
	//	.addDesiredEffect(beliefs["Nothing"])
	//	.buildShared()
	//);
}