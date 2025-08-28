#pragma once
#include <stack>
#include <list>
#include "Module.hpp"

class AgentGoal;
class AgentAction;

class ActionPlan
{
private:
	const std::weak_ptr<AgentGoal> agentGoal;
	Stack<std::weak_ptr<AgentAction>> actions;

public:
	float totalCost = 0.0f;

public:
	ActionPlan() = delete;
	ActionPlan(const std::weak_ptr<AgentGoal> goal, const Stack<std::weak_ptr<AgentAction>>& actions, float totalCost);
	ActionPlan(const std::weak_ptr<AgentGoal> goal, Stack<std::weak_ptr<AgentAction>>&& actions, float totalCost);

	const std::weak_ptr<AgentGoal> getGoal() const;
	Stack<std::weak_ptr<AgentAction>>& getActions();
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


class Node
{
private:
	const std::weak_ptr<Node> parent;
	const std::weak_ptr<AgentAction> action;
	WeakPtrUnorderedSet<AgentBelief> requiredEffects;
	std::list<std::shared_ptr<Node>> leaves;
	float cost = 1.0f;

public:
	Node() = delete;
	Node(const std::weak_ptr<Node>& parent, const std::weak_ptr<AgentAction>& action, const WeakPtrUnorderedSet<AgentBelief>& effects, float cost);
	Node(const std::weak_ptr<Node>& parent, const std::weak_ptr<AgentAction>& action, WeakPtrUnorderedSet<AgentBelief>&& effects, float cost);
	~Node();

	const std::weak_ptr<Node>& getParent() const;
	const std::weak_ptr<AgentAction>& getAction() const;
	WeakPtrUnorderedSet<AgentBelief>& getRequiredEffects();
	std::list<std::shared_ptr<Node>>& getLeaves();
	float getCost() const;

	bool isLeafDead() const;
};