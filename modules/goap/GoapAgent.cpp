#include "GoapAgent.hpp"

Position GoapAgent::getPosition() const
{
	if (!this->position) return Position();
	return this->position();
}