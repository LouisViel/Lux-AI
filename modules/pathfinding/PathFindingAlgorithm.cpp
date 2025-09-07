#include "PathfindingAlgorithm.hpp";
#include "ReservationTable.hpp"

void IPathfindingMultiAlgorithm::applyReservations(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable)
{
	for (std::map<int, AgentPath>::const_iterator it = paths.cbegin(); it != paths.cend(); ++it) {
		applyReservation(it->second, reservationTable);
	}
}

void IPathfindingMultiAlgorithm::applyReservation(const AgentPath& path, ReservationTable& reservationTable)
{
	if (path.isValid()) {
		for (const std::pair<Position, int>& path : path)
			reservationTable.reserve(path.first, path.second);
	} else if (!path.empty()) {
		const Position& pos = path.front().first;
		if (reservationTable.isValid(pos, -1))
			reservationTable.reserve(pos, -1);
	}
}

void IPathfindingMultiAlgorithm::removeReservations(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable)
{
	for (std::map<int, AgentPath>::const_iterator it = paths.cbegin(); it != paths.cend(); ++it) {
		removeReservation(it->second, reservationTable);
	}
}

void IPathfindingMultiAlgorithm::removeReservation(const AgentPath& path, ReservationTable& reservationTable)
{
	if (path.isValid()) {
		for (const std::pair<Position, int>& path : path)
			reservationTable.free(path.first, path.second);
	} else if (!path.empty()) {
		const Position& pos = path.front().first;
		if (reservationTable.isValid(pos, -1))
			reservationTable.free(pos, -1);
	}
}