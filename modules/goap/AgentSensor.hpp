#pragma once
#include "Module.hpp"

class AgentSensor
{
public:
	Event onTargetChanged;

public:
	virtual ~AgentSensor() { }
	virtual bool isTargetInRange() const = 0;
	virtual Position getTargetPosition() const = 0;
	virtual bool isTarget() const = 0;
};