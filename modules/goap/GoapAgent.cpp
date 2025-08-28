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