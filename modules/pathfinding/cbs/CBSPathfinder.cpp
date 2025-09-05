#include "CBSPathfinder.hpp"
#include <queue>
#include "../ReservationTable.hpp"

std::map<int, AgentPath> CBSPathfinder::computePaths(const std::vector<AgentInput>& agents, int time, ReservationTable& reservationTable)
{
    // Declare types & help functions
    typedef std::shared_ptr<CBSPathfinder::Node> NodePtr;
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
        else root->paths[agent.id] = AgentPath();
    }

    // Register root node
    root->heuristic = evaluateHeuristic(root, reservationTable);
    openedQueue.push(root);

    // Loop over opened nodes to solve all conflicts
    int exploredNodes = 0;
    bool checkNodeCount = maxNodes >= 0;
    while (!openedQueue.empty()) {
        NodePtr node = openedQueue.top();
        openedQueue.pop();

        // Try detecting conflicts, otherwise finished !
        std::unique_ptr<Conflict> conflict = detectConflict(node->paths);
        if (!conflict) conflict = detectExternalConflict(node->paths, reservationTable);
        if (!conflict) return node->paths;

        // Safety to avoid infinite, too consuming processing
        if (checkNodeCount && ++exploredNodes > maxNodes) {
            return std::move(getPartialSolution(agents, bestNodeSoFar->paths));
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
            child->heuristic = evaluateHeuristic(child, reservationTable);

            // Register child node
            // Check by cost if node is better than the saved one
            if (child->cost <= bestNodeSoFar->cost) bestNodeSoFar = child;
            openedQueue.push(child);
        }
    }

    // Pas de solution trouvée, returning partial one from the best results
	return std::move(getPartialSolution(agents, bestNodeSoFar->paths));
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


std::unique_ptr<Conflict> CBSPathfinder::detectConflict(const std::map<int, AgentPath>& paths) const
{
    // on parcourt tous les agents
    for (std::map<int, AgentPath>::const_iterator it1 = paths.begin(); it1 != paths.end(); ++it1) {
        for (std::map<int, AgentPath>::const_iterator it2 = std::next(it1); it2 != paths.end(); ++it2) {
            const AgentPath& p1 = it1->second, &p2 = it2->second;
            int id1 = it1->first, id2 = it2->first;

            // Compare positions at each action/time, return conflict if found one
            int p1Size = (int)p1.size(), p2Size = (int)p2.size();
            int T = std::max(p1Size, p2Size);
            for (int t = 0; t < T; ++t) {
                Position pos1 = (t < p1Size) ? p1[t].first : p1.back().first;
                Position pos2 = (t < p2Size) ? p2[t].first : p2.back().first;

                // Cas 1 : même position au même temps (collision directe)
                if (pos1 == pos2 && !isSharedAllowed(id1, id2, pos1, t))
                    return make_unique<Conflict>(id1, id2, pos1, t);

                // Cas 2 et 3 : mouvements entre t -> t+1
                int t1 = t + 1;
                if (t1 < T) {
                    Position next1 = (t1 < p1Size) ? p1[t1].first : p1.back().first;
                    Position next2 = (t1 < p2Size) ? p2[t1].first : p2.back().first;

                    // Cas 3 : swap autorisé, pas de conflit
                    if (pos1 == next2 && pos2 == next1) continue;

                    // Cas 2 : tentative d’entrer sur la case occupée par l’autre
                    if (next1 == pos2 && !isSharedAllowed(id1, id2, next1, t))
                        return make_unique<Conflict>(id1, id2, next1, t1);
                    if (next2 == pos1 && !isSharedAllowed(id1, id2, next2, t))
                        return make_unique<Conflict>(id2, id1, next2, t1);
                }
            }
        }
    }

    // No conflict founded
    return nullptr;
}


std::unique_ptr<Conflict> CBSPathfinder::detectExternalConflict(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable) const
{
    // WARNING : Ne supporte pas les swaps de position !! (du a l'implementation de ReservationTable anonyme)
    // On parcourt tous les agents
    for (const std::pair<const int, AgentPath>& kv : paths) {
        const AgentPath& path = kv.second;
        int id = kv.first;

        // Compare positions at each action/time, return conflict if found one
        for (const std::pair<Position, int>& step : path) {
            const Position& pos = step.first;
            int t = step.second;

            // Check conflit avec une réservation externe (au tour ciblé)
            if (!reservationTable.isFree(pos, t) && !isSharedAllowed(id, -1, pos, t))
                return make_unique<Conflict>(id, -1, pos, t);

            int t0 = t - 1; // Check conflit avec une réservation externe (a l'état initial du tour)
            if (t0 >= 0 && !reservationTable.isFree(pos, t0) && !isSharedAllowed(id, -1, pos, t0))
                return make_unique<Conflict>(id, -1, pos, t);
        }
    }

    // No conflict founded
    return nullptr;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool CBSPathfinder::isSharedAllowed(int id1, int id2, const Position& pos, int time) const
{
    if (!this->sharedAllowed) return false;
    return this->sharedAllowed(id1, id2, pos, time);
}


std::map<int, AgentPath> CBSPathfinder::getPartialSolution(const std::vector<AgentInput>& agents, const std::map<int, AgentPath>& paths) const
{
    std::map<int, AgentPath> partialSolution;
    for (const std::pair<const int, AgentPath>& kv : paths) partialSolution[kv.first] = kv.second; // chemins valides
    for (const AgentInput& agent : agents) {
        if (partialSolution.find(agent.id) == partialSolution.end())
            partialSolution[agent.id] = AgentPath(); // path vide pour ceux bloqué
    } return partialSolution;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


float CBSPathfinder::evaluateHeuristic(const std::shared_ptr<CBSPathfinder::Node>& node, ReservationTable& reservationTable) const
{
    // Estimation des conflits encore présents
    int conflicts = countConflicts(node->paths);
    conflicts += countExternalConflicts(node->paths, reservationTable);
    float h = std::fmax(heuristicAlpha, 0.0f) * conflicts;
    return node->cost + h; // Heuristique : coût + pondération des conflits
}

int CBSPathfinder::countConflicts(const std::map<int, AgentPath>& paths) const
{
    // on parcourt tous les agents
    int conflicts = 0;
    for (std::map<int, AgentPath>::const_iterator it1 = paths.begin(); it1 != paths.end(); ++it1) {
        for (std::map<int, AgentPath>::const_iterator it2 = std::next(it1); it2 != paths.end(); ++it2) {
            const AgentPath& p1 = it1->second, & p2 = it2->second;
            int id1 = it1->first, id2 = it2->first;

            // Compare positions at each action/time, return conflict if found one
            int p1Size = (int)p1.size(), p2Size = (int)p2.size();
            int T = std::max(p1Size, p2Size);
            for (int t = 0; t < T; ++t) {
                Position pos1 = (t < p1Size) ? p1[t].first : p1.back().first;
                Position pos2 = (t < p2Size) ? p2[t].first : p2.back().first;

                // Cas 1 : même position au même temps (collision directe)
                if (pos1 == pos2 && !isSharedAllowed(id1, id2, pos1, t)) {
                    ++conflicts;
                    continue;
                }

                // Cas 2 et 3 : mouvements entre t -> t+1
                int t1 = t + 1;
                if (t1 < T) {
                    Position next1 = (t1 < p1Size) ? p1[t1].first : p1.back().first;
                    Position next2 = (t1 < p2Size) ? p2[t1].first : p2.back().first;

                    // Cas 3 : swap autorisé, pas de conflit
                    if (pos1 == next2 && pos2 == next1) continue;

                    // Cas 2 : tentative d’entrer sur la case occupée par l’autre
                    if ((next1 == pos2 && !isSharedAllowed(id1, id2, next1, t)) ||
                        (next2 == pos1 && !isSharedAllowed(id1, id2, next2, t))
                    ) { ++conflicts; continue; }
                }
            }
        }
    }

    // Return conflicts count
    return conflicts;
}


int CBSPathfinder::countExternalConflicts(const std::map<int, AgentPath>& paths, ReservationTable& reservationTable) const
{
    // WARNING : Ne supporte pas les swaps de position !! (du a l'implementation de ReservationTable anonyme)
    // On parcourt tous les agents
    int conflicts = 0;
    for (const std::pair<const int, AgentPath>& kv : paths) {
        const AgentPath& path = kv.second;
        int id = kv.first;

        // Compare positions at each action/time, return conflict if found one
        for (const std::pair<Position, int>& step : path) {
            const Position& pos = step.first;
            int t = step.second;

            // Check conflit avec une réservation externe (au tour ciblé)
            int t0 = t - 1; // Check conflit avec une réservation externe (a l'état initial du tour)
            if (!reservationTable.isFree(pos, t) && !isSharedAllowed(id, -1, pos, t)) { ++conflicts; continue; }
            if (t0 >= 0 && !reservationTable.isFree(pos, t0) && !isSharedAllowed(id, -1, pos, t0)) { ++conflicts; continue; }
        }
    }

    // Return conflicts count
    return conflicts;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


float CBSPathfinder::getCost(const AgentPath& path) const
{
    // Basic implementation, return action count as cost
    // Can be upgraded providing an internal calculated cost
    // return static_cast<float>(path.size());
    if (path.isValid()) return path.getCost();
    return path.evaluate(static_cast<float>(MAX_TURNS + MAX_TURNS));
}

Func1<float, float> CBSPathfinder::getCostModifier(const AgentInput& agent) const
{
    float costPriority = std::fmax(agent.priority, 0.0f);
    return std::move([costPriority](float cost) {
        return cost * std::powf(0.94f, costPriority);
    });
}


//int CBSPathfinder::findOtherAgent(const Constraint& conflict, const std::map<int, AgentPath>& paths) const
//{
//    // Iterate over all agent and their paths & try to find the wanted one
//    for (std::map<int, AgentPath>::const_iterator it = paths.begin(); it != paths.end(); ++it) {
//        int id = it->first;
//        if (id == conflict.agentId) continue;
//
//        // Try get agent position
//        const AgentPath& path = it->second;
//        const std::pair<bool, Position> res = getPositionAtTime(path, conflict.time);
//
//        // Check results & if match return pos
//        if (!res.first) continue;
//        if (res.second == conflict.pos) return id;
//    }
//
//    // Not match founded, other agent not registered
//    return -1;
//}

// helper : position d'un agent au temps t (dernière position si t >= path.size())
// retourne std::pair<bool, Position> : bool = false si path vide
//std::pair<bool, Position> CBSPathfinder::getPositionAtTime(const AgentPath& path, int t)
//{
//    if (path.empty()) return std::make_pair(false, Position());
//    if (t < static_cast<int>(path.size())) return std::make_pair(true, path[t].first);
//    return std::make_pair(true, path.back().first);
//}
