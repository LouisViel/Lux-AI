#include "UnitHandler.hpp"
#include "project/LuxHelper.hpp"

UnitHandler::UnitHandler(std::string id) : GoapHandler(id),
	nightFuel(0), roadFreeAction(0) { }


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void UnitHandler::setupBeliefs()
{
	// Global beliefs
	factory->addBelief("Nothing", []() { return false; });


	// Nights beliefs
	factory->addBelief("SurviveNight", [this]() {
		ACCESS_UNIT;
		return LuxHelper::surviveNight(LuxHelper::nightDuration, this->nightFuel, unit->cargo);
	});
	factory->addInvert("CannotSurviveNight", "SurviveNight");
	/*factory->addBelief("NightCostOptimal", [this]() {
		ACCESS_UNIT;
		const lux::Cargo& cargo = unit->cargo;
		return LuxHelper::nightCostOptimal(LuxHelper::nightDuration, this->nightFuel, cargo.wood);
	});*/
	factory->addBelief("IsNight", []() {
		return LuxHelper::isNight();
	});


	// City beliefs
	factory->addBelief("InCity", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isCity(unit->pos);
	});
	factory->addInvert("NotInCity", "InCity");


	// Inventory beliefs
	factory->addBelief("InventoryFull", [this]() {
		return this->isFullInventory();
	});
	factory->addInvert("InventoryNotFull", "InventoryFull");
	factory->addBelief("InventoryEmpty", [this]() {
		ACCESS_UNIT;
		const lux::Cargo& cargo = unit->cargo;
		return cargo.wood <= 0 &&
			cargo.coal <= 0 &&
			cargo.uranium <= 0;
	});
	factory->addInvert("InventoryFilled", "InventoryEmpty");
	/*factory->addBelief("InventoryTransfered", [this]() {
		return this->transfered;
	});*/


	// Transport beliefs
	factory->addBelief("TransportCoal", [this]() {
		ACCESS_UNIT;
		return unit->cargo.coal > 0;
	});
	factory->addBelief("TransportUranium", [this]() {
		ACCESS_UNIT;
		return unit->cargo.uranium > 0;
	});
	factory->addBelief("CanReceiveResource", [this]() {
		ACCESS_UNIT;
		return unit->getCargoSpaceLeft() > 0;
	});


	// Road beliefs
	factory->addBelief("OnRoad", [this]() {
		ACCESS_UNIT;
		return LuxHelper::isRoad(unit->pos);
	});
	factory->addBelief("RoadFreeAction", [this]() {
		ACCESS_UNIT;
		return LuxHelper::getRoad(unit->pos) >= this->roadFreeAction;
	});
	factory->addBelief("RoadMax", [this]() {
		ACCESS_UNIT;
		return LuxHelper::getRoad(unit->pos) >= LuxHelper::maxRoad;
	});
	factory->addBelief("RoadFriendly", []() { return false; });
	factory->addBelief("RoadNonFriendly", []() { return false; });
	//factory->addInvert("RoadNonFriendly", "RoadFriendly");


	// Cooldown beliefs
	factory->addBelief("HasCooldown", [this]() {
		ACCESS_UNIT;
		return unit->cooldown > 0;
	});
	factory->addInvert("NoCooldown", "HasCooldown");


	// Move beliefs
	factory->addBelief("HasPath", [this]() {
		return this->hasPath;
	});
	factory->addBelief("CanMove", [this]() {
		ACCESS_UNIT;
		return !LuxHelper::isBlocked(unit->pos);
	});
	factory->addBelief("Moved", [this]() {
		return this->moved;
	});


	// Experimental beliefs
	//factory->addBelief("IsSurviving", []() { return false; });
	//factory->addBelief("IsInDanger", []() { return false; });
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void UnitHandler::setupActions()
{
	ACCESS_BELIEFS;

	agent->actions->insert(AgentAction::Builder("Idle")
		.withStrategy(utils::make_unique<IdleStrategy>())
		.addEffect(beliefs["Nothing"])
		.withCost(1.0f)
		.buildShared()
	);

	//// TODO : C'est vraiment utile ???? Pas plutot integrer dans les strategies ???
	//agent->actions->insert(AgentAction::Builder("PrepareMove")
	//	.withStrategy(nullptr) // TODO : IMPLEMENT --------------------------------
	//	.addPrecondition(beliefs["CanMove"])
	//	.addEffect(beliefs["HasPath"])
	//	.withCost(0.0f)
	//	.buildShared()
	//);

	//// TODO : C'est vraiment utile ???? Pas plutot integrer dans les strategies ???
	//agent->actions->insert(AgentAction::Builder("Move")
	//	.withStrategy(nullptr) // TODO : IMPLEMENT --------------------------------
	//	.addPrecondition(beliefs["HasPath"])
	//	.addEffect(beliefs["Moved"])
	//	.withCost(1.0f)
	//	.buildShared()
	//);

	// TODO : C'est vraiment utile ???? Pas plutot integrer ailleurs ???
	//agent->actions->insert(AgentAction::Builder("Transfer")
	//	.withStrategy(nullptr) // TODO : IMPLEMENT --------------------------------
	//	.addPrecondition(beliefs["InventoryFilled"])
	//	.addEffect(beliefs["InventoryTransfered"])
	//	.withCost(1.0f)
	//	.buildShared()
	//);

	agent->actions->insert(AgentAction::Builder("SurviveNight")
		.withStrategy(nullptr)
		.addEffect(beliefs["SurviveNight"])
		.withCost(1.0f)
		.buildShared()
	);

	agent->actions->insert(AgentAction::Builder("SupplyCity")
		.withStrategy(nullptr)
		.addEffect(beliefs["InventoryFilled"])
		.addEffect(beliefs["InCity"])
		.withCost(1.0f)
		.buildShared()
	);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void UnitHandler::setupGoals()
{
	ACCESS_BELIEFS;

	agent->goals->insert(AgentGoal::Builder("SurviveNight")
		.withPriority(1)
		.addDesiredEffect(beliefs["SurviveNight"])
		.buildShared()
	);

	/*agent->goals->insert(AgentGoal::Builder("FullInventory")
		.withPriority(1)
		.addDesiredEffect(beliefs["InventoryFull"])
		.buildShared()
	);*/

	agent->goals->insert(AgentGoal::Builder("SupplyCity")
		.withPriority(1) // TODO : Implement City Manager telling who need & how much + effect based on distance & assignements already
		.addDesiredEffect(beliefs["InventoryFilled"])
		.addDesiredEffect(beliefs["InCity"])
		.buildShared()
	);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool UnitHandler::isFullInventory() const
{
	ACCESS_UNIT;
	return unit->getCargoSpaceLeft() <= 0;
}