#pragma once
#include <string>
#include <functional>
#include "Module.hpp"

class GoapAgent
{
private:
	FuncPosition position;

public:
	Position getPosition() const;
};

