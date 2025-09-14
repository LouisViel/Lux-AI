#include "HandlersManager.h"

GoapHandler* HandlersManager::getHandler(std::string id)
{
	for(GoapHandler* handler : handlers)
	{
		if (handler->getId() == id)
			return handler;
	}
	return nullptr;
}

std::vector<GoapHandler*> HandlersManager::handlers;

void HandlersManager::addHandler(GoapHandler* handler)
{
	handlers.push_back(handler);
}