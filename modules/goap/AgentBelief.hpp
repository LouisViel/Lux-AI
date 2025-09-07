#pragma once
#include <string>
#include <functional>
#include "Module.hpp"

class AgentBelief
{
private:
	const std::string name;
	std::function<bool()> condition;
	std::function<Position()> observedLocation;

public:
	AgentBelief() = delete;
	// Template - AgentBelief(std::string name);
	~AgentBelief();

	const std::string& getName() const;
	Position getLocation() const;
	bool isLocation() const;
	bool evaluate() const;

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

private:
	make_unique_friend
	template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
	AgentBelief(Str&& name) : name(std::forward<Str>(name))
	{
		this->condition = []() { return false; };
		this->observedLocation = []() { return Position(); };
	}


	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////


public:
	class Builder
	{
	private:
		std::unique_ptr<AgentBelief> belief;
		bool built = false;

	public:
		Builder() = delete;
		// Template - Builder(std::string name);
		~Builder();

		Builder(const Builder&) = delete;
		Builder& operator=(const Builder&) = delete;
		Builder(Builder&&) = default;
		Builder& operator=(Builder&&) = default;

		AgentBelief::Builder& withCondition(FuncBool&& condition);
		AgentBelief::Builder& withCondition(const FuncBool& condition);
		AgentBelief::Builder& withLocation(FuncPosition&& location);
		AgentBelief::Builder& withLocation(const FuncPosition& location);

		std::unique_ptr<AgentBelief> build();
		std::shared_ptr<AgentBelief> buildShared();

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////

	public:
		template<typename Str, typename = typename std::enable_if<std::is_convertible<Str, std::string>::value>::type>
		Builder(Str&& name) : belief(utils::make_unique<AgentBelief>(std::forward<Str>(name)))
		{
			// Empty Constructor
		}
	};
};