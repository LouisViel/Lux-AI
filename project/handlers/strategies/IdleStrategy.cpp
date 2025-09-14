#include "IdleStrategy.hpp"

bool IdleStrategy::canPerform() const
{
    return true;
}

bool IdleStrategy::isComplete() const
{
    return true;
}

void IdleStrategy::start()
{
    // Empty
}

void IdleStrategy::update(int turnId)
{
    // Empty
}

void IdleStrategy::stop()
{
    // Empty
}