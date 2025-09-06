#include "PathCache.hpp"
#include <random>

PathCache::PathCache(std::shared_ptr<IPathfindingAlgorithm> algorithm) :
    algorithm(algorithm), cacheVersion(0),
    versionSalt(std::random_device()())
{ }

void PathCache::reset()
{
    cache.clear();
    ++cacheVersion;
    versionSalt = std::random_device()();
}

PathKey PathCache::key(const AgentInput& agent, const Constraints& constraints) const
{
    return PathKey(agent.id, hashConstraints(constraints));
}

PathPtr PathCache::get(const AgentInput& agent, const Constraints& constraints) const
{
    PathKey key = this->key(agent, constraints);
    return get(key);
}

PathPtr PathCache::get(PathKey key) const
{
    auto it = cache.find(key);
    if (it != cache.end()) return it->second.lock();
    return nullptr;
}

// WARNING ! Call compute only if optmizing by doing things manually. Otherwise use getOrCompute
PathPtr PathCache::compute(const AgentInput& agent, const Constraints& constraints, int time, const std::weak_ptr<ReservationTable>& rtWeak)
{
    PathKey key = this->key(agent, constraints);
    return compute(key, agent, constraints, time, rtWeak);
}

// WARNING ! Call compute only if optmizing by doing things manually. Otherwise use getOrCompute
PathPtr PathCache::compute(const PathKey& key, const AgentInput& agent, const Constraints& constraints, int time, const std::weak_ptr<ReservationTable>& rtWeak)
{
    // Compute path from internal algorithm
    AgentPath newPath = algorithm->computePath(agent, time, constraints, rtWeak);
    if (!newPath.isValid()) newPath = AgentPath::invalid(agent.start, time);

    // Create sharedPtr AgentPath with smart deleter
    std::weak_ptr<PathCache> self = shared_from_this();
    PathPtr ptr(new AgentPath(std::move(newPath)),
        [self, key](AgentPath* path) {
            if (std::shared_ptr<PathCache> ptr = self.lock())
                ptr->cache.erase(key);
            delete path;
        }
    );

    // Cache AgentPath as WeakPtr & Return it
    cache[key] = PathWeak(ptr);
    return ptr;
}

PathPtr PathCache::getOrCompute(const AgentInput& agent, const Constraints& constraints, int time, const std::weak_ptr<ReservationTable>& rtWeak, Func1<float, float> modifier)
{
    PathKey key = this->key(agent, constraints);
    PathPtr fromCache = get(key);
    if (fromCache) return fromCache;
    PathPtr computed = compute(key, agent, constraints, time, rtWeak);
    computed->addModifier(modifier);
    return computed;
}

std::size_t PathCache::hashConstraints(const Constraints& constraints) const
{
    std::size_t seed = cacheVersion ^ versionSalt;
    for (const Constraint& c : constraints) {
        seed ^= std::hash<int>()(c.agentId) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(c.pos.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(c.pos.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(c.time) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    } return seed;
}