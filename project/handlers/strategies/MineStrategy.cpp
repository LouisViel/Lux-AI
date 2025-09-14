#include "MineStrategy.hpp"

MineStrategy::MineStrategy(WorkerHandler* worker) : worker(worker)
{

}

bool MineStrategy::canPerform() const
{
	return false;
}

bool MineStrategy::isComplete() const
{
	return false;
}

void MineStrategy::start()
{

}

void MineStrategy::update(int turnId)
{

}

void MineStrategy::stop() const
{

}