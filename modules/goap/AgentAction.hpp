#pragma once
#include <string>
#include "Module.hpp"

class AgentBelief;
class IActionStrategy;

class AgentAction
{
private:

private:
	const std::string name;
	float cost = 0.0f;

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
		AgentAction::Builder& withStrategy(std::unique_ptr<IActionStrategy> strategy);
		AgentAction::Builder& withPrecondition(std::weak_ptr<AgentBelief> precondition);
		AgentAction::Builder& withEffect(std::weak_ptr<AgentBelief> effect);

		std::unique_ptr<AgentAction> build();
		std::shared_ptr<AgentAction> buildShared();

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	public:
		template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
		Builder(Str&& name) : action(make_unique<AgentAction>(std::forward<Str>(name)))
		{
			action->cost = 1;
		}
	};
};
