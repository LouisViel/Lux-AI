#include "GoapHandler.hpp"
#include "modules/goap/GoapPlanner.hpp"

GoapHandler::GoapHandler() :
	agent(std::make_shared<GoapAgent>()),
	factory(utils::make_unique<BeliefFactory>(agent, agent->beliefs))
{
	setupBeliefs();
	setupActions();
	setupGoals();
}

void GoapHandler::resetPlan()
{
	agent->currentGoal = std::weak_ptr<AgentGoal>();
	agent->currentAction = std::weak_ptr<AgentAction>();
}

void GoapHandler::update(int time)
{
	// Get access to current action
	std::shared_ptr<AgentAction> currentAction = agent->currentAction.lock();

	// Update the plan and current action if there is none
	if (!currentAction) {
		calculatePlan();

		// Check if we got a valid plan & actions
		const std::shared_ptr<ActionPlan>& actionPlan = agent->actionPlan;
		if (actionPlan) {
			Stack<std::weak_ptr<AgentAction>>& actions = actionPlan->getActions();
			if (!actions.empty()) {

				// Register goal & action
				agent->currentGoal = actionPlan->getGoal();
				agent->currentAction = actions.top();
				actions.pop();

				// Start the new action if possible
				currentAction = agent->currentAction.lock();
				if (currentAction && currentAction->canStart())
					currentAction->start();

				// otherwise reset to nothing
				else {
					agent->currentGoal = std::weak_ptr<AgentGoal>();
					agent->currentAction = std::weak_ptr<AgentAction>();
					currentAction = std::shared_ptr<AgentAction>();
				}	
			}
		}
	}

	// If we have an action, update it
	if (agent->actionPlan && currentAction) {
		currentAction->update(time);

		// Stop action if it is complete
		if (currentAction->isComplete()) {
			currentAction->stop();
			agent->currentAction = std::weak_ptr<AgentAction>();

			// Check if we finish the full plan
			if (agent->actionPlan->getActions().empty()) {
				agent->lastGoal = agent->currentGoal;
				agent->currentGoal = std::weak_ptr<AgentGoal>();
			}
		}
	}
}

void GoapHandler::calculatePlan()
{
	// Get or Select Goals to check based on current priority (> current if running already)
	std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>> goalsToCheck = agent->goals;
	if (const std::shared_ptr<AgentGoal> currentGoal = agent->currentGoal.lock()) {
		float priorityLevel = currentGoal->getPriority();
		
		// Copy all goals that have a priority over current one
		goalsToCheck = std::make_shared<SharedPtrUnorderedSet<AgentGoal>>();
		std::copy_if(agent->goals->begin(), agent->goals->end(),
			std::inserter(*goalsToCheck, goalsToCheck->end()),
			[&priorityLevel](std::shared_ptr<AgentGoal> goal) {
				return goal->getPriority() > priorityLevel;
			}
		);
	}

	// Plan potential plan & Register it if valid
	std::shared_ptr<ActionPlan> potentialPlan = agent->gPlanner->plan(agent, goalsToCheck, agent->lastGoal);
	if (potentialPlan) agent->actionPlan = std::move(potentialPlan);
}