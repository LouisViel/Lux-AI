#include "ActionPlan.hpp"
#include "AgentAction.hpp"
#include "AgentGoal.hpp"


ActionPlan::ActionPlan(const std::weak_ptr<AgentGoal> goal, const Stack<std::weak_ptr<AgentAction>>& actions, float totalCost)
	: agentGoal(goal), actions(actions), totalCost(totalCost) { }

ActionPlan::ActionPlan(const std::weak_ptr<AgentGoal> goal, Stack<std::weak_ptr<AgentAction>>&& actions, float totalCost)
	: agentGoal(goal), actions(std::move(actions)), totalCost(totalCost) { }

const std::weak_ptr<AgentGoal> ActionPlan::getGoal() const
{
	return agentGoal;
}

Stack<std::weak_ptr<AgentAction>>& ActionPlan::getActions()
{
	return actions;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


Node::Node(const std::weak_ptr<Node>& parent, const std::weak_ptr<AgentAction>& action, const WeakPtrUnorderedSet<AgentBelief>& effects, float cost)
	: parent(parent), action(action), requiredEffects(effects), cost(cost) { }

Node::Node(const std::weak_ptr<Node>& parent, const std::weak_ptr<AgentAction>& action, WeakPtrUnorderedSet<AgentBelief>&& effects, float cost)
	: parent(parent), action(action), requiredEffects(std::move(effects)), cost(cost) { }

Node::~Node() { }

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

const std::weak_ptr<Node>& Node::getParent() const
{
	return parent;
}

const std::weak_ptr<AgentAction>& Node::getAction() const
{
	return action;
}

WeakPtrUnorderedSet<AgentBelief>& Node::getRequiredEffects()
{
	return requiredEffects;
}

std::list<std::shared_ptr<Node>>& Node::getLeaves()
{
	return leaves;
}

float Node::getCost() const
{
	return cost;
}

bool Node::isLeafDead() const
{
	return leaves.size() == 0 && action.expired();
}