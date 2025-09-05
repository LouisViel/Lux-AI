#include "CBSPathfinder.hpp"
#include <queue>
#include "../ReservationTable.hpp"


std::map<int, AgentPath> CBSPathfinder::computePaths(const std::vector<AgentInput>& agents, int time, ReservationTable& reservationTable)
{
    // Declare types & help functions
    struct NodeCmp {
        bool operator()(const std::shared_ptr<CBSPathfinder::Node>& a, const std::shared_ptr<CBSPathfinder::Node>& b) const {
            return a->heuristic > b->heuristic; // min-heap
        }
    };

    // Cast reservation table to shared/weak pointer
    // Trick to use empty lambda as shared_ptr destructor instead of default one
    std::shared_ptr<ReservationTable> rtShared(&reservationTable, [](ReservationTable*) { });
    std::weak_ptr<ReservationTable> rtWeak = rtShared;

    // Create Priority queue pour explorer nodes &  Root node
    std::priority_queue<NodePtr, std::vector<NodePtr>, NodeCmp> openedQueue;
    NodePtr root = std::make_shared<CBSPathfinder::Node>();
    NodePtr bestNodeSoFar = root;

    // Calcul initial des chemins pour tous les agents
    Constraints emptyConstraints;
    for (const AgentInput& agent : agents) {
        AgentPath path = std::move(algorithm->computePath(agent, time, emptyConstraints, rtWeak));

        // Inject cost modifiers
        path.addModifier(getCostModifier(agent));
        root->cost += getCost(path);

        // Register path, valid or not
        if (path.isValid()) root->paths[agent.id] = std::move(path);
        else root->paths[agent.id] = AgentPath(1, std::make_pair(agent.start, time));
    }

    // Register root node
    // Evaluate first time node conflicts & heuristic
    root->heuristic = evaluateHeuristic(root, reservationTable);
    openedQueue.push(root);

    // Loop over opened nodes to solve all conflicts
    int exploredNodes = 0;
    bool checkNodeCount = maxNodes >= 0;
    while (!openedQueue.empty()) {
        NodePtr node = openedQueue.top();
        openedQueue.pop();

        // Try detecting conflicts, otherwise finished !
        const std::unique_ptr<Conflict>& conflict = node->lastConflict;
        if (!conflict) return node->paths;

        // Safety to avoid infinite, too consuming processing
        if (checkNodeCount && ++exploredNodes > maxNodes) {
            return std::move(getPartialSolution(agents, time, bestNodeSoFar->paths));
        }

        // Générer deux contraintes (un par agent impliqué dans le conflit)
        Constraint c1(conflict->agentA, conflict->pos, conflict->time);
        Constraint c2(conflict->agentB, conflict->pos, conflict->time);

        // Loop over chaque nouvelle contrainte
        for (const Constraint& constraint : { c1, c2 }) {
            if (constraint.agentId == -1) continue;

            // Create conflict child node & add constraint
            NodePtr child = std::make_shared<CBSPathfinder::Node>(*node);
            child->constraints.push_back(constraint);

            // Substract path cost from node if it exists
            std::map<int, AgentPath>::const_iterator pathIt = child->paths.find(constraint.agentId);
            if (pathIt != child->paths.end()) child->cost -= getCost(pathIt->second);

            // Find & Recalculer chemin pour l’agent concerné
            const AgentInput& agent = *std::find_if(agents.begin(), agents.end(), [&constraint](const AgentInput& a) { return a.id == constraint.agentId; });
            AgentPath newPath = std::move(algorithm->computePath(agent, time, child->constraints, rtWeak));
            
            // Inject cost modifiers
            newPath.addModifier(getCostModifier(agent));
            
            // Ensure path is valid & register it
            if (!newPath.isValid()) continue; // echec du replanning
            child->cost += getCost(newPath);
            child->paths[constraint.agentId] = std::move(newPath);
            
            // Update node conflicts & heuristic
            updateConflicts(child, constraint.agentId, reservationTable);
            child->heuristic = evaluateHeuristic(child, reservationTable);

            // Register child node
            // Check by cost if node is better than the saved one
            if (child->cost <= bestNodeSoFar->cost) bestNodeSoFar = child;
            openedQueue.push(child);
        }
    }

    // Pas de solution trouvée, returning partial one from the best results
	return std::move(getPartialSolution(agents, time, bestNodeSoFar->paths));
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool CBSPathfinder::isSharedAllowed(int id1, int id2, const Position& pos, int time) const
{
    if (!this->sharedAllowed) return false;
    return this->sharedAllowed(id1, id2, pos, time);
}

std::map<int, AgentPath> CBSPathfinder::getPartialSolution(const std::vector<AgentInput>& agents, int time, const std::map<int, AgentPath>& paths) const
{
    std::map<int, AgentPath> partialSolution;
    for (const std::pair<const int, AgentPath>& kv : paths) partialSolution[kv.first] = kv.second; // chemins valides
    for (const AgentInput& agent : agents) {
        if (partialSolution.find(agent.id) == partialSolution.end())
            partialSolution[agent.id] = AgentPath(1, std::make_pair(agent.start, time)); // path vide pour ceux bloqué
    } return partialSolution;
}

float CBSPathfinder::getCost(const AgentPath& path) const
{
    // Return path processed cost if valid, otherwise MAX_TURNS x 2
    if (path.isValid()) return path.getCost();
    return path.evaluate(static_cast<float>(MAX_TURNS + MAX_TURNS));
}

Func1<float, float> CBSPathfinder::getCostModifier(const AgentInput& agent) const
{
    // Return a lambda function to modify exponentially cost by priority
    float costPriority = std::fmax(agent.priority, 0.0f);
    return std::move([costPriority](float cost) {
        return cost * std::powf(0.94f, costPriority);
    });
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


float CBSPathfinder::evaluateHeuristic(const NodePtr& node, ReservationTable& reservationTable) const
{
    // Calculates conflicts if not already done
    if (node->conflictCount < 0) {
        if (heuristicAlpha > 0.0f) {
            node->lastConflict = nullptr;
            node->conflicts.clear();
            node->conflictCount = countConflictAndCache(node) + countEConflictAndCache(node, reservationTable);
        } else heuristicFallback(node, reservationTable);
    } float h = std::fmax(heuristicAlpha, 0.0f) * node->conflictCount;
    return node->cost + h; // Heuristique : coût + pondération des conflits
}

void CBSPathfinder::updateConflicts(const NodePtr& node, int changedAgent, ReservationTable& reservationTable) const
{
    // Prevent huge processing if heuristic not used
    if (heuristicAlpha <= 0.0f) {
        heuristicFallback(node, reservationTable);
        return;
    }

    // Initialize conflicts counter
    if (changedAgent == -1) return;
    const AgentPath& agentPath = node->paths.at(changedAgent);
    node->conflictCount -= node->conflicts.getAndRemove(changedAgent);
    node->lastConflict = nullptr;
    int conflicts = 0;

    // Conflits internes (l'agent contre les autres) && externes (reservation table)
    for (const std::pair<const int, AgentPath>& kv : node->paths) {
        if (kv.first == changedAgent) continue;
        conflicts += countConflictPairAndCache(node, agentPath, kv.second, changedAgent, kv.first);
    } conflicts += countEConflictAgentAndCache(node, agentPath, changedAgent, reservationTable);

    // Save & apply conflicts count
    node->conflictCount += conflicts;

    // If all conflicts for this agent are solved, try find a new conflict
    if (node->conflictCount > 0 && !node->lastConflict) {
        node->lastConflict = detectConflict(node->paths);
        if (!node->lastConflict) node->lastConflict = detectEConflict(node->paths, reservationTable);
    }
}

void CBSPathfinder::heuristicFallback(const NodePtr& node, ReservationTable& reservationTable) const
{
    // Try detecting conflicts
    node->lastConflict = detectConflict(node->paths);
    if (!node->lastConflict) node->lastConflict = detectEConflict(node->paths, reservationTable);

    // Apply conflicts counts values
    node->conflicts.clear();
    if (node->lastConflict) {
        node->conflicts[ConflictCore(*node->lastConflict)] = 1;
        node->conflictCount = 1;
    } else node->conflictCount = 0;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


std::unique_ptr<Conflict> CBSPathfinder::detectConflict(const std::map<int, AgentPath>& paths) const
{
    // Setup conflict pointer & callback function
    std::unique_ptr<Conflict> conflict = nullptr;
    Func4<bool, int, int, Position, int> callback = [&conflict](const int& id1, const int& id2, const Position& pos, const int& t) {
        conflict = make_unique<Conflict>(id1, id2, pos, t);
        return true;
    };

    // Loop over all path pairs & try to find a conflict
    for (std::map<int, AgentPath>::const_iterator it1 = paths.begin(); it1 != paths.end(); ++it1) {
        for (std::map<int, AgentPath>::const_iterator it2 = std::next(it1); it2 != paths.end(); ++it2) {
            forEachConflict(it1->second, it2->second, it1->first, it2->first, callback);
            if (conflict) return conflict;
        }
    } return nullptr;
}

int CBSPathfinder::countConflictAndCache(const NodePtr& node) const
{
    // Setup references
    const std::map<int, AgentPath>& paths = node->paths;
    std::unique_ptr<Conflict>& lastConflict = node->lastConflict;
    ConflictMap& conflicts = node->conflicts;

    // Setup conflicts counter & callback function
    int conflictCount = 0;
    Func4<bool, int, int, Position, int> callback = [&conflictCount, &lastConflict, &conflicts]
        (const int& id1, const int& id2, const Position& pos, const int& t) {
            if (!lastConflict) lastConflict = make_unique<Conflict>(id1, id2, pos, t);
            ++conflicts[ConflictCore(id1, id2)];
            ++conflictCount;
            return false;
    };
    
    // Loop over all path pairs & count its conflicts
    for (std::map<int, AgentPath>::const_iterator it1 = paths.begin(); it1 != paths.end(); ++it1) {
        for (std::map<int, AgentPath>::const_iterator it2 = std::next(it1); it2 != paths.end(); ++it2) {
            forEachConflict(it1->second, it2->second, it1->first, it2->first, callback);
        }
    } return conflictCount;
}

int CBSPathfinder::countConflictPairAndCache(const NodePtr& node, const AgentPath& p1, const AgentPath& p2, int id1, int id2) const
{
    // Setup references
    const std::map<int, AgentPath>& paths = node->paths;
    std::unique_ptr<Conflict>& lastConflict = node->lastConflict;
    ConflictMap& conflicts = node->conflicts;

    // Count pair conflicts and cache first found
    int conflictCount = 0;
    forEachConflict(p1, p2, id1, id2, [&conflictCount, &lastConflict, &conflicts](const int& id1, const int& id2, const Position& pos, const int& t) {
        if (!lastConflict) lastConflict = make_unique<Conflict>(id1, id2, pos, t);
        ++conflicts[ConflictCore(id1, id2)];
        ++conflictCount;
        return false;
    });
    return conflictCount;
}

void CBSPathfinder::forEachConflict(const AgentPath& p1, const AgentPath& p2, int id1, int id2, const Func4<bool, int, int, Position, int>& callback) const
{
    // Compare positions at each action/time, invoke callback when find a conflict
    int p1Size = (int)p1.size(), p2Size = (int)p2.size();
    int T = std::max(p1Size, p2Size);
    for (int t = 0; t < T; ++t) {
        Position pos1 = (t < p1Size) ? p1[t].first : p1.back().first;
        Position pos2 = (t < p2Size) ? p2[t].first : p2.back().first;

        // Cas 1 : même position au même temps (collision directe)
        if (pos1 == pos2 && !isSharedAllowed(id1, id2, pos1, t)) {
            if (callback(id1, id2, pos1, t)) return;
            continue;
        }

        // Cas 2 et 3 : mouvements entre t -> t+1
        int t1 = t + 1;
        if (t1 < T) {
            Position next1 = (t1 < p1Size) ? p1[t1].first : p1.back().first;
            Position next2 = (t1 < p2Size) ? p2[t1].first : p2.back().first;

            // Cas 3 : swap autorisé, pas de conflit
            if (pos1 == next2 && pos2 == next1) continue;

            // Cas 2 - a : tentative d’entrer sur la case occupée par l’autre
            if (next1 == pos2 && !isSharedAllowed(id1, id2, next1, t)) {
                if (callback(id1, id2, next1, t1)) return;
                continue;
            }

            // Cas 2 - b : tentative d’entrer sur la case occupée par l’autre
            if (next2 == pos1 && !isSharedAllowed(id1, id2, next2, t)) {
                if (callback(id2, id1, next2, t1)) return;
                continue;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


// WARNING : Ne supporte pas les swaps de position !! (du a l'implementation de ReservationTable anonyme)
std::unique_ptr<Conflict> CBSPathfinder::detectEConflict(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable) const
{
    // Setup conflict pointer & callback function
    std::unique_ptr<Conflict> conflict = nullptr;
    const Func4<bool, int, int, Position, int> callback = [&conflict](const int& id1, const int& id2, const Position& pos, const int& t) {
        conflict = make_unique<Conflict>(id1, id2, pos, t);
        return true;
    };
    
    // On parcourt tous les agents & try to find a conflict
    for (const std::pair<const int, AgentPath>& kv : paths) {
        forEachEConflict(kv.second, kv.first, reservationTable, callback);
        if (conflict) return conflict;
    } return nullptr;
}

// WARNING : Ne supporte pas les swaps de position !! (du a l'implementation de ReservationTable anonyme)
int CBSPathfinder::countEConflictAndCache(const NodePtr& node, ReservationTable& reservationTable) const
{
    // Setup references
    const std::map<int, AgentPath>& paths = node->paths;
    std::unique_ptr<Conflict>& lastConflict = node->lastConflict;
    ConflictMap& conflicts = node->conflicts;

    // Setup conflicts counter & callback function
    int conflictCount = 0;
    const Func4<bool, int, int, Position, int> callback = [&conflictCount, &lastConflict, &conflicts](const int& id1, const int& id2, const Position& pos, const int& t) {
        if (!lastConflict) lastConflict = make_unique<Conflict>(id1, id2, pos, t);
        ++conflicts[ConflictCore(id1, id2)];
        ++conflictCount;
        return false;
    };

    // Loop over all path pairs & count its conflicts
    for (const std::pair<const int, AgentPath>& kv : paths) {
        forEachEConflict(kv.second, kv.first, reservationTable, callback);
    } return conflictCount;
}

int CBSPathfinder::countEConflictAgentAndCache(const NodePtr& node, const AgentPath& path, int id, ReservationTable& reservationTable) const
{
    // Setup references
    const std::map<int, AgentPath>& paths = node->paths;
    std::unique_ptr<Conflict>& lastConflict = node->lastConflict;
    ConflictMap& conflicts = node->conflicts;

    // Count agent conflicts & cache first found
    int conflictCount = 0;
    forEachEConflict(path, id, reservationTable, [&conflictCount, &lastConflict, &conflicts](const int& id1, const int& id2, const Position& pos, const int& t) {
        if (!lastConflict) lastConflict = make_unique<Conflict>(id1, id2, pos, t);
        ++conflicts[ConflictCore(id1, id2)];
        ++conflictCount;
        return false;
    });
    return conflictCount;
}

void CBSPathfinder::forEachEConflict(const AgentPath& path, int id, ReservationTable& rt, const Func4<bool, int, int, Position, int>& callback) const
{
    // Compare positions at each action/time, invoke callback when find a conflict
    for (const std::pair<Position, int>& step : path) {
        const Position& pos = step.first;
        int t = step.second;

        // Check conflit avec une réservation externe (au tour ciblé)
        if (!rt.isFree(pos, t) && !isSharedAllowed(id, -1, pos, t)) {
            if (callback(id, -1, pos, t)) return;
            continue;
        }

        int t0 = t - 1; // Check conflit avec une réservation externe (a l'état initial du tour)
        if (t0 >= 0 && !rt.isFree(pos, t0) && !isSharedAllowed(id, -1, pos, t0)) {
            if (callback(id, -1, pos, t)) return;
            continue;
        }
    }
}
