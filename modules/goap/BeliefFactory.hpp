#pragma once
#include "AgentBelief.hpp"
class GoapAgent;
class AgentSensor;

class BeliefFactory
{
private:
	const std::weak_ptr<GoapAgent> agent;
	std::map<std::string, std::shared_ptr<AgentBelief>> beliefs;

public:
	BeliefFactory() = delete;
	BeliefFactory(std::weak_ptr<GoapAgent> agent, std::map<std::string, std::shared_ptr<AgentBelief>> beliefs);
	~BeliefFactory();

	// Template - void addBelief(std::string key, FuncBool condition);
	// Template - void addLocationBelief(std::string key, float distance, FuncPosition locationProvider);
	void addLocationBelief(const std::string& key, float distance, Position locationCondition);
	void addSensorBelief(const std::string& key, std::weak_ptr<AgentSensor> sensor);

private:
	bool inRangeOf(Position pos, float range) const;

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

public:
	template<typename Func, typename = typename std::enable_if<std::is_convertible<Func, FuncBool>::value>::type>
	void addBelief(const std::string& key, Func&& condition)
	{
		beliefs.insert(std::make_pair(key, AgentBelief::Builder(key)
			.withCondition(std::forward<Func>(condition))
			.buildShared()
		));
	}

	// Surcharge auto-complétion lvalue
	inline void addBelief(const std::string& key, const FuncBool& condition)
	{
		addBelief(key, condition);
	}

	// Surcharge auto-complétion rvalue
	inline void addBelief(const std::string& key, FuncBool&& condition)
	{
		addBelief(key, std::move(condition));
	}

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	template<typename Func, typename = typename std::enable_if<std::is_convertible<Func, FuncPosition>::value>::type>
	void addLocationBelief(const std::string& key, float distance, Func&& locationProvider)
	{
		std::shared_ptr<FuncPosition> locProv = std::make_shared<FuncPosition>(std::forward<Func>(locationProvider));
		beliefs.insert(std::make_pair(key, AgentBelief::Builder(key)

			.withCondition([locProv, distance, this]() {
				FuncPosition& provider = *locProv;
				if (!provider) return false;
				return inRangeOf(provider(), distance);

			}).withLocation([locProv]() {
				FuncPosition& provider = *locProv;
				if (!provider) return Position();
				return provider();

			}).buildShared()
		));
	}

	// Surcharge auto-complétion lvalue
	inline void addLocationBelief(const std::string& key, float distance, const FuncPosition& locationProvider)
	{
		addLocationBelief(key, distance, locationProvider);
	}

	// Surcharge auto-complétion rvalue
	inline void addLocationBelief(const std::string& key, float distance, FuncPosition&& locationProvider)
	{
		addLocationBelief(key, distance, std::move(locationProvider));
	}
};