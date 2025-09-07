#pragma once
#include <string>
#include <functional>
#include "Module.hpp"

class AgentGoal;
class AgentAction;
class AgentBelief;
class ActionPlan;
class IGoapPlanner;

class GoapAgent
{
protected:
	FuncPosition position;
	Func1<float, Position> path;
	float pathCost = -1.0f;

public:
	const std::shared_ptr<BeliefMap> beliefs;
	const std::shared_ptr<SharedPtrUnorderedSet<AgentAction>> actions;
	const std::shared_ptr<SharedPtrUnorderedSet<AgentGoal>> goals;

	std::weak_ptr<AgentGoal> lastGoal;
	std::weak_ptr<AgentGoal> currentGoal;
	std::weak_ptr<AgentAction> currentAction;

	std::shared_ptr<ActionPlan> actionPlan = nullptr;
	std::shared_ptr<IGoapPlanner> gPlanner = nullptr;

public:
	GoapAgent();
	~GoapAgent();

	// Template - void setPosition(const FuncPosition& position);
	// Template - void setPath(const Func<float, Position>& path);

	Position getPosition() const;
	float getPathCost() const;
	float calculatePath(Position target);


	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////


	template <typename T, typename = typename std::enable_if<std::is_convertible<T, FuncPosition>::value>::type>
	void setPosition(T&& pposition)
	{
		this->position = std::forward<T>(position);
	}

	// Surcharge auto-complétion lvalue
	inline void setPosition(const FuncPosition& position)
	{
		setPosition(position);
	}

	// Surcharge auto-complétion rvalue
	inline void setPosition(FuncPosition&& position)
	{
		setPosition(std::move(position));
	}

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	template <typename T, typename = typename std::enable_if<std::is_convertible<T, Func1<float, Position>>::value>::type>
	void setPath(T&& path)
	{
		this->path = std::forward<T>(path);
	}

	// Surcharge auto-complétion lvalue
	inline void setPath(const Func1<float, Position>& path)
	{
		setPath(path);
	}

	// Surcharge auto-complétion rvalue
	inline void setPath(Func1<float, Position>&& path)
	{
		setPath(std::move(path));
	}
};