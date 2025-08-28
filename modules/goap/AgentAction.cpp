#include "AgentAction.hpp"
#include "AgentBelief.hpp"
#include "IActionStrategy.hpp"


AgentAction::~AgentAction()
{
	// Empty Destructor
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


const std::string& AgentAction::getName() const
{
	return name;
}

float AgentAction::getCost() const
{
	return cost;
}

bool AgentAction::isComplete() const
{
	if (!strategy) return true;
	return strategy->isComplete();
}

const WeakPtrUnorderedSet<AgentBelief>& AgentAction::getPreconditions() const
{
	return preconditions;
}

const WeakPtrUnorderedSet<AgentBelief>& AgentAction::getEffects() const
{
	return effects;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void AgentAction::start()
{
	if (!strategy) return;
	strategy->start();
}

void AgentAction::update(int turnId)
{
	if (!strategy) return;
	if (strategy->canPerform())
		strategy->update(turnId);

	if (!strategy->isComplete()) return;
	for (const std::weak_ptr<AgentBelief>& effect : effects) {
		if (std::shared_ptr<AgentBelief> ptr = effect.lock())
			ptr->evaluate();
	}
}

void AgentAction::stop()
{
	if (!strategy) return;
	strategy->stop();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AgentAction::Builder::~Builder() { }

AgentAction::Builder& AgentAction::Builder::withCost(float cost)
{
	if (built) throw std::runtime_error("Builder already built");
	action->cost = cost;
	return *this;
}

AgentAction::Builder& AgentAction::Builder::withStrategy(std::unique_ptr<IActionStrategy> strategy)
{
	if (built) throw std::runtime_error("Builder already built");
	action->strategy = std::move(strategy);
	return *this;
}

AgentAction::Builder& AgentAction::Builder::withPrecondition(std::weak_ptr<AgentBelief> precondition)
{
	if (built) throw std::runtime_error("Builder already built");
	action->preconditions.insert(precondition);
	return *this;
}

AgentAction::Builder& AgentAction::Builder::withEffect(std::weak_ptr<AgentBelief> effect)
{
	if (built) throw std::runtime_error("Builder already built");
	action->effects.insert(effect);
	return *this;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

std::unique_ptr<AgentAction> AgentAction::Builder::build()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::move(action);
}

std::shared_ptr<AgentAction> AgentAction::Builder::buildShared()
{
	if (built) throw std::runtime_error("Builder already built");
	built = true;
	return std::shared_ptr<AgentAction>(std::move(action));
}