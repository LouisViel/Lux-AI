#include "ReservationTable.hpp"

ReservationTable::ReservationTable(int width, int height) :
	reservations(width, std::vector<std::bitset<MAX_TURNS>>(height)),
	width(width), height(height)
{ }

ReservationTable::~ReservationTable()
{
	// Empty destructor
}

bool ReservationTable::isFree(const Position& pos, int t) const
{
	if (!ensure(pos, t)) return false;
	if (t < 0) return reservations[pos.x][pos.y].none();
	return !reservations[pos.x][pos.y].test(t);
}

void ReservationTable::reserve(const Position& pos, int t)
{
	if (t < 0) reservations[pos.x][pos.y].set();
	else reservations[pos.x][pos.y].set(t);
}

void ReservationTable::free(const Position& pos, int t)
{
	if (t < 0) reservations[pos.x][pos.y].reset();
	else reservations[pos.x][pos.y].reset(t);
}

void ReservationTable::reset()
{
	for (size_t x = 0; x < width; ++x) {
		for (size_t y = 0; y < height; ++y) {
			reservations[x][y].reset();
		}
	}
}

bool ReservationTable::isValid(const Position& pos, int t) const
{
	return ensure(pos, t);
}

inline bool ReservationTable::ensure(const Position& pos, int t) const
{
	return (pos.x >= 0 && pos.x < width) &&
		(pos.y >= 0 && pos.y < height) &&
		(/*t >= 0 &&*/ t < MAX_TURNS);
}
