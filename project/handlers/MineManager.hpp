#pragma once
#include "lux/kit.hpp"
#include <map>
#include <unordered_set>

namespace std {
    template <>
    struct hash<lux::Position> {
        size_t operator()(const lux::Position &pos) const noexcept {
            return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
        }
    };
}

class MineManager
{
private:
	std::unordered_map<std::string, lux::Position> buffer;
	std::unordered_set<lux::Position> woodNodes;
	std::unordered_set<lux::Position> coalNodes;
	std::unordered_set<lux::Position> uraniumNodes;

public:
	MineManager(const std::vector<std::vector<lux::Cell>>& map);
	~MineManager() = default;

	const lux::Position getTarget(std::string agentId, const lux::Position& start, bool wood = true, bool coal = true, bool uranium = true);
	void remove(const std::string& agentId);

private:
	bool isValid(const lux::Position& pos);
	double computeScore(const lux::Position& from, const lux::Position& to, double value);
};