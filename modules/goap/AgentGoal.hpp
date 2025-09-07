#pragma once
#include <string>
#include "Module.hpp"

class AgentBelief;

class AgentGoal
{
private:
	const std::string name;
	WeakPtrUnorderedSet<AgentBelief> desiredEffects;
	Func<float> priority;

public:
	AgentGoal() = delete;
	// Template - AgentGoal(std::string name);
	~AgentGoal();

	const std::string& getName() const;
	float getPriority() const;
	const WeakPtrUnorderedSet<AgentBelief>& getDesiredEffects() const;

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

private:
	make_unique_friend
	template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
	AgentGoal(Str&& name) : name(std::forward<Str>(name))
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
		std::unique_ptr<AgentGoal> goal;
		bool built = false;

	public:
		Builder() = delete;
		// Template - Builder(std::string name);
		~Builder();

		AgentGoal::Builder& withPriority(float priority);
		// Template - AgentGoal::Builder& withPriority(Func<float> priority);
		AgentGoal::Builder& withDesiredEffect(std::weak_ptr<AgentBelief> effect);

		std::unique_ptr<AgentGoal> build();
		std::shared_ptr<AgentGoal> buildShared();

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////

	public:
		template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
		Builder(Str&& name) : goal(make_unique<AgentGoal>(std::forward<Str>(name)))
		{
			goal->priority = []() { return 0.0f; };
		}

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////

		template<typename FuncFloat, typename = typename std::enable_if<std::is_convertible<FuncFloat, Func<float>>::value>::type>
		AgentGoal::Builder& withPriority(FuncFloat&& priority)
		{
			if (built) throw std::runtime_error("Builder already built");
			goal->priority = std::forward<FuncFloat>(priority);
			return *this;
		}

		// Surcharge auto-complétion lvalue
		inline AgentGoal::Builder& withPriority(const Func<float>& priority)
		{
			return withPriority(priority);
		}

		// Surcharge auto-complétion rvalue
		inline AgentGoal::Builder& withPriority(Func<float>&& priority)
		{
			return withPriority(priority);
		}
	};
};
