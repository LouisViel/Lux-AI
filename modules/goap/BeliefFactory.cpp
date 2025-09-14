#include "BeliefFactory.hpp"
#include "GoapAgent.hpp"
#include "AgentSensor.hpp"
#include "AgentBelief.hpp"


BeliefFactory::BeliefFactory(const std::shared_ptr<GoapAgent> agent, const std::shared_ptr<BeliefMap> beliefs)
	: BeliefFactory(std::weak_ptr<GoapAgent>(agent), BeliefMapPtr(beliefs)) { }

BeliefFactory::BeliefFactory(const std::weak_ptr<GoapAgent> agent, const BeliefMapPtr beliefs)
	: agent(agent), beliefs(beliefs) { }

BeliefFactory::~BeliefFactory() { }


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void BeliefFactory::addInvert(const std::string& key, const std::string& invert)
{
	std::shared_ptr<BeliefMap> beliefPtr = beliefs.lock();
	if (!beliefPtr) return;
	auto it = beliefPtr->find(invert);
	if (it == beliefPtr->end()) return;

	const std::weak_ptr<AgentBelief> belief = it->second;
	addBelief(key, [belief]() {
		if (std::shared_ptr<AgentBelief> ptr = belief.lock()) {
			return !ptr->evaluate();
		} return false;
	});
}

void BeliefFactory::addLocationBelief(const std::string& key, float distance, Position locationCondition)
{
	std::shared_ptr<BeliefMap> beliefPtr = beliefs.lock();
	if (!beliefPtr) return;

	beliefPtr->insert(std::make_pair(key, AgentBelief::Builder(key)
		.withCondition([locationCondition, distance, this]() { return inRangeOf(locationCondition, distance); })
		.withLocation([locationCondition]() { return locationCondition; })
		.buildShared()
	));
}

void BeliefFactory::addSensorBelief(const std::string& key, std::weak_ptr<AgentSensor> sensor)
{
	std::shared_ptr<BeliefMap> beliefPtr = beliefs.lock();
	if (!beliefPtr) return;

	beliefPtr->insert(std::make_pair(key, AgentBelief::Builder(key)
		.withCondition([sensor]() {
			if (std::shared_ptr<AgentSensor> ptr = sensor.lock()) {
				return ptr->isTargetInRange();
			} return false;
		
		}).withLocation([sensor]() {
			if (std::shared_ptr<AgentSensor> ptr = sensor.lock()) {
				return ptr->getTargetPosition();
			} return Position();

		}).buildShared()
	));
}

bool BeliefFactory::inRangeOf(Position pos, float range) const
{
	if (std::shared_ptr<GoapAgent> ptr = agent.lock()) {
		return ptr->getPosition().distanceTo(pos) <= range;
	} return false;
}
