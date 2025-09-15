#include "CreateCartStrategy.h"
#include "../handlers/CityHandler.hpp"
#include "../../LuxHelper.hpp"
#include "utils.hpp"

CreateCartStrategy::CreateCartStrategy(CityHandler* cityHandler)
{
    handler = cityHandler;
}
bool CreateCartStrategy::canPerform() const
{
    return handler->getTile()->cooldown < 1;
}

bool CreateCartStrategy::isComplete() const
{
    return true;
}

void CreateCartStrategy::start()
{
    ACCESS_OUTPUT;
    const lux::CityTile* tile = handler->getTile();
    luxOutput.push_back(tile->buildCart());
    handler->cartCreated++;
}

void CreateCartStrategy::update(int turnId)
{

}

void CreateCartStrategy::stop()
{
    // Empty
}