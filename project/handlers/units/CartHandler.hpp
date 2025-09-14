#pragma once
#include "../handlers/UnitHandler.hpp"

class CartHandler : public UnitHandler
{
public:
	using UnitHandler::UnitHandler;

	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;
};

