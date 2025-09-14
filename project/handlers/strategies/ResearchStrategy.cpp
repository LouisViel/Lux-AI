#include "ResearchStrategy.h"
#include "../handlers/CityHandler.hpp"
#include "../../LuxHelper.hpp"
#include "utils.hpp"

ResearchStrategy::ResearchStrategy(CityHandler* cityHandler)
{
    handler = cityHandler;
}
bool ResearchStrategy::canPerform() const
{
    return handler->getTile()->cooldown < 1;
}

bool ResearchStrategy::isComplete() const
{
    return true;
}

void ResearchStrategy::start()
{
    ACCESS_OUTPUT;
    const lux::CityTile* tile = handler->getTile();
    luxOutput.push_back(tile->research());
}

void ResearchStrategy::update(int turnId)
{

}

void ResearchStrategy::stop() const
{
    // Empty
}