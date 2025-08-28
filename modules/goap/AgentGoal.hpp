#pragma once
#include <string>
#include "Module.hpp"

class AgentBelief;

class AgentGoal
{
private:
	const std::string name;
	float priority;
	WeakPtrUnorderedSet<AgentBelief> desiredEffects;

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
		AgentGoal::Builder& withDesiredEffect(std::weak_ptr<AgentBelief> effect);

		std::unique_ptr<AgentGoal> build();
		std::shared_ptr<AgentGoal> buildShared();

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////

	public:
		template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
		Builder(Str&& name) : goal(make_unique<AgentGoal>(std::forward<Str>(name)))
		{
			// Empty Constructor
		}
	};
};
