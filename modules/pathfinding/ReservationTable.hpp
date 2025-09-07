#pragma once
#include <vector>
#include <bitset>
#include "Module.hpp"

class ReservationTable
{
private:
	int width, height;
	std::vector<std::vector<std::bitset<MAX_TURNS>>> reservations;

public:
	ReservationTable() = delete;
	ReservationTable(int width, int height);
	~ReservationTable();

	bool isValid(const Position& pos, int t = -1) const;
	bool isFree(const Position& pos, int t = -1) const;
	void reserve(const Position& pos, int t = -1);
	void free(const Position& pos, int t = -1);
	void reset();

private:
	inline bool ensure(const Position& pos, int t) const;
};