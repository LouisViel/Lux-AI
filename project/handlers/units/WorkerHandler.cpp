#include "WorkerHandler.hpp"
#include "project/LuxHelper.hpp"
#include "../strategies/MineStrategy.hpp"

void WorkerHandler::setupBeliefs()
{
	UnitHandler::setupBeliefs();
	ACCESS_BELIEFS;

	// Mining beliefs
	factory->addBelief("Mining", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isMining(unit->pos);
	});
	/*factory->addBelief("MiningWood", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isMining(unit->pos, lux::ResourceType::wood);
	});
	factory->addBelief("MiningCoal", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isMining(unit->pos, lux::ResourceType::coal);
	});
	factory->addBelief("MiningUranium", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isMining(unit->pos, lux::ResourceType::uranium);
	});*/


	// Research beliefs
	/*factory->addBelief("CanMineCoal", []() {
		return LuxHelper::canMineCoal();
	});
	factory->addBelief("CanMineUranium", []() {
		return LuxHelper::canMineUranium();
	});*/


	// Action beliefs
	//factory->addBelief("CanBuildCity", [this]() {
	//	ACCESS_UNIT;
	//	return unit->canBuild(*LuxHelper::gameMap); // TODO : Revoir si on évalue pas ce behaviour juste avec si l'inventaire est full
	//});
	factory->addBelief("CityBuild", [this]() {
		return this->cityBuild;
	});
	factory->addBelief("RoadDestroyed", [this]() {
		return this->destroyedRoad;
	});
}

void WorkerHandler::setupActions()
{
	UnitHandler::setupActions();
	ACCESS_BELIEFS;

	agent->actions->insert(AgentAction::Builder("MineAction")
		.withStrategy(utils::make_unique<MineStrategy>(this))
		//.addPrecondition(beliefs["InventoryNotFull"])
		//.addEffect(beliefs["InventoryFull"])
		.addEffect(beliefs["InventoryFilled"])
		.withCost(1.0f)
		.buildShared()
	);

	//agent->actions->insert(AgentAction::Builder("Pillage")
	//	.withStrategy(nullptr)  // TODO : Implement
	//	.addPrecondition(beliefs["RoadNonFriendly"])
	//	.addEffect(beliefs["RoadDestroyed"])
	//	.withCost(1.0f) // TODO : Implement
	//	.buildShared()
	//);

	//agent->actions->insert(AgentAction::Builder("BuildCity")
	//	.withStrategy(nullptr)  // TODO : Implement
	//	.addPrecondition(beliefs["CanBuildCity"])
	//	.addEffect(beliefs["CityBuild"])
	//	.withCost(1.0f) // TODO : Implement
	//	.buildShared()
	//);
}

void WorkerHandler::setupGoals()
{
	//ACCESS_OUTPUT;
	//luxOutput.push_back(LuxHelper::getUnit(id)->move(lux::DIRECTIONS::NORTH));

	UnitHandler::setupGoals();
	ACCESS_BELIEFS;

	agent->goals->insert(AgentGoal::Builder("Mine")
		.withPriority(2.0f)
		//.addDesiredEffect(beliefs["InventoryFull"])
		.addDesiredEffect(beliefs["InventoryFilled"])
		//.addDesiredEffect(beliefs["SurviveNight"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("BuildCity")
		.withPriority(1.0f) // TODO : Implement
		.addDesiredEffect(beliefs["CityBuild"])
		.buildShared()
	);

	agent->goals->insert(AgentGoal::Builder("DestroyRoad")
		.withPriority(1.0f) // TODO : Implement
		.addDesiredEffect(beliefs["RoadNonFriendly"])
		.addDesiredEffect(beliefs["RoadDestroyed"])
		.buildShared()
	);
}

bool WorkerHandler::isMining() const
{
	ACCESS_UNIT;
	return LuxHelper::isMining(unit->pos);
}

bool WorkerHandler::isMining(const lux::ResourceType resource) const
{
	ACCESS_UNIT;
	return LuxHelper::isMining(unit->pos, resource);
}

bool WorkerHandler::canMove() const
{
	ACCESS_UNIT;
	return !LuxHelper::isBlocked(unit->pos);
}
