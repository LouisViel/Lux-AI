#pragma once
#include <string>
#include <functional>
#include "Module.hpp"

class AgentGoal;
class AgentAction;
class AgentBelief;
class ActionPlan;

class GoapAgent
{
private:
	FuncPosition position;

public:
	const std::shared_ptr<BeliefMap> beliefs;
	const std::shared_ptr<SharedPtrUnorderedSet<AgentAction>> actions;
	const std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>> goals;

	std::weak_ptr<AgentGoal> lastGoal;
	std::weak_ptr<AgentGoal> currentGoal;
	std::weak_ptr<AgentAction> currentAction;
	std::shared_ptr<ActionPlan> actionPlan;

public:
	GoapAgent();
	~GoapAgent();
	Position getPosition() const;
};