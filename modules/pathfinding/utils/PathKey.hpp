#pragma once
#include "../Module.hpp"

struct PathKey
{
public:
	int agentId;
	size_t constraint;

	PathKey(int agentId, size_t constraint) : agentId(agentId), constraint(constraint) { }

	bool operator==(const PathKey& other) const {
		return agentId == other.agentId && constraint == other.constraint;
	}
};

struct PathKeyHasher {
	std::size_t operator()(const PathKey& k) const {
		return std::hash<int>()(k.agentId) ^ (k.constraint << 1);
	}
};