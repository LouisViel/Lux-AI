#pragma once
#include "../Module.hpp"
#include "../PathFindingAlgorithm.hpp"
#include "Constraint.hpp"
#include "PathKey.hpp"

using PathPtr = std::shared_ptr<AgentPath>;

class PathCache : std::enable_shared_from_this<PathCache>
{
private:
    using PathWeak = std::weak_ptr<AgentPath>;
    std::shared_ptr<IPathfindingAlgorithm> algorithm;
    std::unordered_map<PathKey, PathWeak, PathKeyHasher> cache;
    size_t cacheVersion, versionSalt;

public:
    PathCache(std::shared_ptr<IPathfindingAlgorithm> algorithm);
    void reset();

    PathKey key(const AgentInput& agent, const Constraints& constraints) const;
    PathPtr get(const AgentInput& agent, const Constraints& constraints) const;
    PathPtr get(PathKey key) const;

    PathPtr compute(
        const AgentInput& agent,
        const Constraints& constraints,
        int time,
        const std::weak_ptr<ReservationTable>& rtWeak
    );

    PathPtr compute(
        const PathKey& key,
        const AgentInput& agent,
        const Constraints& constraints,
        int time,
        const std::weak_ptr<ReservationTable>& rtWeak
    );

    PathPtr getOrCompute(
        const AgentInput& agent,
        const Constraints& constraints,
        int time,
        const std::weak_ptr<ReservationTable>& rtWeak,
        Func1<float, float> modifier
    );

private:
    std::size_t hashConstraints(const Constraints& constraints) const;
};