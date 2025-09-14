#include "CreateWorkerSrategy.h"
#include "../handlers/CityHandler.hpp"
#include "../../LuxHelper.hpp"
#include "utils.hpp"

CreateWorkerSrategy::CreateWorkerSrategy(CityHandler* cityHandler)
{
    handler = cityHandler;
}
bool CreateWorkerSrategy::canPerform() const
{
    return handler->getTile()->cooldown < 1;
}

bool CreateWorkerSrategy::isComplete() const
{
    return true;
}

void CreateWorkerSrategy::start()
{
    ACCESS_OUTPUT;
    const lux::CityTile* tile = handler->getTile();
    luxOutput.push_back(tile->buildWorker());
    handler->createdWorker++;
}

void CreateWorkerSrategy::update(int turnId)
{
    
}

void CreateWorkerSrategy::stop() const
{
    // Empty
}