#include "GoapAgent.hpp"

GoapAgent::GoapAgent() :
	beliefs(std::make_shared<BeliefMap>()),
	actions(std::make_shared<SharedPtrUnorderedSet<AgentAction>>()),
	goals(std::make_shared<SharedPtrUnorderedSet<AgentGoal>>())
{ }

GoapAgent::~GoapAgent() { }

Position GoapAgent::getPosition() const
{
	if (!this->position) return Position();
	return this->position();
}

float GoapAgent::getPathCost() const
{
	return pathCost;
}

float GoapAgent::calculatePath(Position target)
{
	if (!this->path) pathCost = -1.0f;
	else pathCost = this->path(target);
	return pathCost;
}
