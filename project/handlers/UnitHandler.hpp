#pragma once
#include "GoapHandler.hpp"

class UnitHandler : public GoapHandler
{
public:
	UnitHandler();
	virtual ~UnitHandler() = default;

	virtual void setupBeliefs() override;
	virtual void setupActions() override;
	virtual void setupGoals() override;
};