#pragma once
#include "GoapHandler.hpp"

class HandlersManager
{
public:
	static std::vector<GoapHandler*> handlers;
	static GoapHandler* getHandler(std::string id);
	static void addHandler(GoapHandler* handler);
	static void removeHandler(GoapHandler* handler);
};

