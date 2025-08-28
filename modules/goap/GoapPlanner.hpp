#pragma once
#include "Module.hpp"
#include "ActionPlan.hpp"

class GoapAgent;
class AgentGoal;
class AgentAction;
class Node;

class IGoapPlanner
{
public:
	virtual ~IGoapPlanner() = default;
	virtual std::shared_ptr<ActionPlan> plan(
		const std::shared_ptr<GoapAgent>& agent,
		const std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>>& goals,
		const std::weak_ptr<AgentGoal> mostRecentGoal = std::weak_ptr<AgentGoal>()
	) = 0;
};

class GoapPlanner : public IGoapPlanner
{
public:
	std::shared_ptr<ActionPlan> plan(
		const std::shared_ptr<GoapAgent>& agent,
		const std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>>& goals,
		const std::weak_ptr<AgentGoal> mostRecentGoal
	) override;

	bool findPath(
		const std::shared_ptr<Node>& parent,
		const SharedPtrUnorderedSet<AgentAction>& actions
	);
};