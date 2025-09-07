#pragma once
#include <string>
#include "Module.hpp"
#include "IActionStrategy.hpp"

class AgentBelief;

class AgentAction
{
private:

private:
	const std::string name;
	Func<float> cost;

	WeakPtrUnorderedSet<AgentBelief> preconditions;
	WeakPtrUnorderedSet<AgentBelief> effects;
	std::unique_ptr<IActionStrategy> strategy;

public:
	AgentAction() = delete;
	// Template - AgentAction(std::string name);
	~AgentAction();

	const std::string& getName() const;
	float getCost() const;
	bool isComplete() const;

	const WeakPtrUnorderedSet<AgentBelief>& getPreconditions() const;
	const WeakPtrUnorderedSet<AgentBelief>& getEffects() const;

	bool canStart() const;
	void start();
	void update(int turnId);
	void stop();
	

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

private:
	make_unique_friend
	template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
	AgentAction(Str&& name) : name(std::forward<Str>(name))
	{
		// Empty Constructor
	}


	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////


public:
	class Builder
	{
	private:
		std::unique_ptr<AgentAction> action;
		bool built = false;

	public:
		Builder() = delete;
		// Template - Builder(std::string name);
		~Builder();

		AgentAction::Builder& withCost(float cost);
		// Template - AgentAction::Builder& withCost(Func<float> cost);
		AgentAction::Builder& withStrategy(std::unique_ptr<IActionStrategy>&& strategy);
		AgentAction::Builder& addPrecondition(std::weak_ptr<AgentBelief> precondition);
		AgentAction::Builder& addEffect(std::weak_ptr<AgentBelief> effect);

		std::unique_ptr<AgentAction> build();
		std::shared_ptr<AgentAction> buildShared();

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	public:
		template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
		Builder(Str&& name) : action(make_unique<AgentAction>(std::forward<Str>(name)))
		{
			action->cost = []() { return 1.0f; };
		}

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////

		template<typename FuncFloat, typename = typename std::enable_if<std::is_convertible<FuncFloat, Func<float>>::value>::type>
		AgentAction::Builder& withCost(FuncFloat&& cost)
		{
			if (built) throw std::runtime_error("Builder already built");
			action->cost = std::forward<FuncFloat>(cost);
			return *this;
		}

		// Surcharge auto-complétion lvalue
		inline AgentAction::Builder& withCost(const Func<float>& cost)
		{
			return withCost(cost);
		}

		// Surcharge auto-complétion rvalue
		inline AgentAction::Builder& withCost(Func<float>&& cost)
		{
			return withCost(cost);
		}
	};
};
