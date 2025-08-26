#include "AgentBelief.hpp"


AgentBelief::~AgentBelief() { }

const std::string& AgentBelief::getName() const
{
	return this->name;
}

Position AgentBelief::getLocation() const
{
	if (!this->observedLocation) return Position();
	return this->observedLocation();
}

bool AgentBelief::isLocation() const
{
	return this->getLocation() != Position();
}

bool AgentBelief::evaluate() const
{
	if (!this->condition) return false;
	return this->condition();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AgentBelief::Builder::~Builder() { }

AgentBelief::Builder& AgentBelief::Builder::withCondition(FuncBool&& condition)
{
	if (built) throw std::runtime_error("Builder already built");
	belief->condition = std::move(condition);
	return *this;
}

AgentBelief::Builder& AgentBelief::Builder::withCondition(const FuncBool& condition)
{
	if (built) throw std::runtime_error("Builder already built");
	belief->condition = condition; // copie
	return *this;
}

AgentBelief::Builder& AgentBelief::Builder::withLocation(FuncPosition&& location)
{
	if (built) throw std::runtime_error("Builder already built");
	belief->observedLocation = std::move(location);
	return *this;
}

AgentBelief::Builder& AgentBelief::Builder::withLocation(const FuncPosition& location)
{
	if (built) throw std::runtime_error("Builder already built");
	belief->observedLocation = location; // copie
	return *this;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

std::unique_ptr<AgentBelief> AgentBelief::Builder::build()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::move(belief);
}

std::shared_ptr<AgentBelief> AgentBelief::Builder::buildShared()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::shared_ptr<AgentBelief>(std::move(belief));
}