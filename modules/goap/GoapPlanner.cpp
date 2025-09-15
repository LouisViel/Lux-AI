#include "GoapPlanner.hpp"
#include "AgentGoal.hpp"
#include "AgentBelief.hpp"
#include "GoapAgent.hpp"
#include "AgentAction.hpp"
#include <list>

std::shared_ptr<ActionPlan> GoapPlanner::plan(const std::shared_ptr<GoapAgent>& agent, const std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>>& goals, const std::weak_ptr<AgentGoal> mostRecentGoal)
{
	std::list<std::shared_ptr<AgentGoal>> orderedGoals;
	for (const std::shared_ptr<AgentGoal>& goal : *goals) {
		
		// Search for all goals that have un-finished desired effects
		const WeakPtrUnorderedSet<AgentBelief>& desiredEffects = goal->getDesiredEffects();
		bool hasEffectNeeded = std::any_of(
			desiredEffects.begin(), desiredEffects.end(),
			[](const std::weak_ptr<AgentBelief>& effect) {
				if (const std::shared_ptr<AgentBelief> ptr = effect.lock()) {
					return !ptr->evaluate();
				} return false;
			}
		);

		if (hasEffectNeeded) {
			orderedGoals.push_back(goal);
		}
	}

	// Sort goals by priority (with most recent one having a penalty)
	if (const std::shared_ptr<AgentGoal> mostRecent = mostRecentGoal.lock()) {
		orderedGoals.sort([&mostRecent](const std::shared_ptr<AgentGoal>& a, const std::shared_ptr<AgentGoal>& b) {
			float aPriority = a->getPriority();
			float bPriority = b->getPriority();
			if (a == mostRecent) aPriority -= 0.01f;
			if (b == mostRecent) bPriority -= 0.01f;
			return aPriority > bPriority;
		});
	} else {
		orderedGoals.sort([](const std::shared_ptr<AgentGoal>& a, const std::shared_ptr<AgentGoal>& b) {
			return a->getPriority() > b->getPriority();
		});
	}

	// Try to solve each goal in order
	for (const std::shared_ptr<AgentGoal>& goal : orderedGoals) {
		std::shared_ptr<Node> goalNode = std::make_shared<Node>(std::weak_ptr<Node>(), std::weak_ptr<AgentAction>(), goal->getDesiredEffects(), 0);

		

		// If we can find a path to the goal, return the plan
		if (findPath(goalNode, *agent->actions)) {

			// If the goalNode has no leaves and no actions to perform, try another goal
			if (goalNode->isLeafDead()) continue;

			// Deconstruct tree into the most optimized plan possible
			std::stack<std::weak_ptr<AgentAction>> actionStack;
			while (!goalNode->getLeaves().empty()) {
				const std::list<std::shared_ptr<Node>>& leaves = goalNode->getLeaves();

				// Find cheapest leaf by iterator
				auto cheapestIt = std::min_element(
					leaves.begin(), leaves.end(),
					[](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
						return a->getCost() < b->getCost();
					}
				);

				// Get cheapest leaf, replace current goalNode with it, and push it's action
				const std::shared_ptr<Node> cheapestLeaf = *cheapestIt;
				actionStack.push(cheapestLeaf->getAction());

				// Warning !! Node is responsible for it's leaves (shared_ptr)
				// Replacing/Destroying it will loose all these other "plans"
				// Carefull later if try to create a new planner or optimize this
				goalNode = cheapestLeaf;
			}

			// Create Action Plan (trick with sharedPtr cause c++11 not happy otherwise)
			return std::make_shared<ActionPlan>(
				std::weak_ptr<AgentGoal>(goal),
				std::move(actionStack),
				goalNode->getCost()
			);
		}
	}

	// No plan founded, need to check before using plan
	return nullptr;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool GoapPlanner::findPath(const std::shared_ptr<Node>& parent, const SharedPtrUnorderedSet<AgentAction>& actions)
{
	//return false;

	// Sort Actions in cost order
	std::vector<std::shared_ptr<AgentAction>> sortedActions(actions.begin(), actions.end());
	
	std::sort(sortedActions.begin(), sortedActions.end(),
		[](const std::shared_ptr<AgentAction>& a, const std::shared_ptr<AgentAction>& b) {
			return a->getCost() < b->getCost();
	});
	return false;
	// Check all actions if there is a path to fulffill parent
	for (const std::shared_ptr<AgentAction>& action : sortedActions) {

		// Remove required effects already fullfilled (RemoveWhere)
		WeakPtrUnorderedSet<AgentBelief>& requiredEffects = parent->getRequiredEffects();
		for (auto it = requiredEffects.begin(); it != requiredEffects.end();) {
			const std::shared_ptr<AgentBelief> ptr = it->lock();
			if (ptr && ptr->evaluate()) it = requiredEffects.erase(it);
			else ++it;
		}
		
		// If there are no required effects to fullfill, plan is finished
		if (requiredEffects.size() == 0) return true;

		// Check if action effects fullfill a requiredEffect
		const WeakPtrUnorderedSet<AgentBelief>& actionEffects = action->getEffects();
		bool hasCommonEffect = std::any_of(
			actionEffects.begin(), actionEffects.end(),
			[&requiredEffects](const std::weak_ptr<AgentBelief>& effect) {
				if (effect.expired()) return false;
				return requiredEffects.count(effect) > 0;
			}
		);

		// action is interesting, going further to have full path
		if (hasCommonEffect) {
			WeakPtrUnorderedSet<AgentBelief> newRequiredEffects = requiredEffects; // copy
			
			// Remove (ExceptWith) required effects the Action will fullfill
			for (auto it = newRequiredEffects.begin(); it != newRequiredEffects.end(); ) {
				if (actionEffects.find(*it) != actionEffects.end())
					it = newRequiredEffects.erase(it);
				else ++it;
			}

			// Merge (UnionWith) required effects with needed ones from the target action
			const WeakPtrUnorderedSet<AgentBelief>& actionPreconditions = action->getPreconditions();
			for (const std::weak_ptr<AgentBelief>& precondition : actionPreconditions) {
				newRequiredEffects.insert(precondition);
			}

			// Remove target action from pool before calling recursion
			// (Can maybe be changed in an other planner ? (we can eat multiple time before doing some exercice for example))
			SharedPtrUnorderedSet<AgentAction> newAvailableActions = actions; // copy
			newAvailableActions.erase(action);

			// Create new child node
			std::shared_ptr<Node> newNode = std::make_shared<Node>(
				std::weak_ptr<Node>(parent),
				std::weak_ptr<AgentAction>(action),
				std::move(newRequiredEffects),
				parent->getCost() + action->getCost()
			);

			// Explore the new node recursively, then refresh potentially updated references (prevent issues if was modified in recursion)
			bool foundedPath = findPath(newNode, newAvailableActions);
			WeakPtrUnorderedSet<AgentBelief>& refreshedRequiredEffects = newNode->getRequiredEffects();

			// If path was found, construct plan at this depth
			if (foundedPath) {
				// Register newNode as a leaf of parent
				parent->getLeaves().push_back(std::move(newNode));

				// Remove from requiredEffects newNode's action preconditions (ExceptWith)
				for (auto it = refreshedRequiredEffects.begin(); it != refreshedRequiredEffects.end(); ) {
					if (actionPreconditions.find(*it) != actionPreconditions.end())
						it = refreshedRequiredEffects.erase(it);
					else ++it;
				}
			}

			// If all effects at this depth have been satisfied, return true plan is complete
			if (refreshedRequiredEffects.empty()) return true;
		}
	}

	// Did not found a path
	return false;
}

