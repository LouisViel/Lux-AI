#pragma once
#include "modules/goap/GoapAgent.hpp"
#include "modules/goap/BeliefFactory.hpp"
#include "modules/goap/AgentAction.hpp"
#include "modules/goap/AgentGoal.hpp"
#include "../strategies/Strategies.hpp"
#include "lux/kit.hpp"

class GoapHandler
{
protected:
	template <typename MapT>
	struct AtWrapper {
		const MapT& map; // référence au conteneur, quel qu'il soit
		//typename MapT::mapped_type& operator[](const typename MapT::key_type& key) { return map.at(key); }
		const typename MapT::mapped_type& operator[](const typename MapT::key_type& key) const { return map.at(key); }
		AtWrapper(const MapT& map) : map(map) {}
		AtWrapper() = delete;
	};

	using Beliefs = AtWrapper<BeliefMap>;
	#define ACCESS_BELIEFS Beliefs beliefs(*agent->beliefs);

protected:
	std::unique_ptr<BeliefFactory> factory;
	std::shared_ptr<GoapAgent> agent;
	std::string id = "-";

protected:
	GoapHandler();
	GoapHandler(std::string id);
	virtual void setupBeliefs() { }
	virtual void setupActions() { }
	virtual void setupGoals() { }
	void resetPlan();

public:
	virtual ~GoapHandler() = default;
	virtual void update(int time);

	const std::string& getId() const;

private:
	void calculatePlan();
};