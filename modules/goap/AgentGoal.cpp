#include "AgentGoal.hpp"


AgentGoal::~AgentGoal() { }

const std::string& AgentGoal::getName() const
{
	return name;
}

float AgentGoal::getPriority() const
{
	return priority;
}

const WeakPtrUnorderedSet<AgentBelief>& AgentGoal::getDesiredEffects() const
{
	return desiredEffects;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AgentGoal::Builder::~Builder() { }

AgentGoal::Builder& AgentGoal::Builder::withPriority(float priority)
{
	if (built) throw std::runtime_error("Builder already built");
	goal->priority = priority;
	return *this;
}

AgentGoal::Builder& AgentGoal::Builder::withDesiredEffect(std::weak_ptr<AgentBelief> effect)
{
	if (built) throw std::runtime_error("Builder already built");
	goal->desiredEffects.insert(effect);
	return *this;
}

std::unique_ptr<AgentGoal> AgentGoal::Builder::build()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::move(goal);
}

std::shared_ptr<AgentGoal> AgentGoal::Builder::buildShared()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::shared_ptr<AgentGoal>(std::move(goal));
}
