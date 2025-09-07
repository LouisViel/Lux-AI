#include "AStarPathfinder.hpp"
#include "../ReservationTable.hpp"
#include <queue>

AgentPath AStarPathfinder::computePath(
	const AgentInput& agent, int time, const Constraints& constraints,
	std::weak_ptr<ReservationTable> reservationTable
) {
	// Ensure goal is not start, otherwise early exit
	if (agent.start == agent.goal) {
		return AgentPath::invalid(agent.start, time);
	}

	// Declare constant helper
	static const Position dirs[5] = {
		Position(1, 0), // right
		Position(-1, 0), // left
		Position(0, 1), // top
		Position(0, -1), // bottom
		Position(0, 0) // wait
	};

	// Declare types & help functions
	typedef std::shared_ptr<AStarPathfinder::Node> NodePtr;
	typedef std::pair<float, NodePtr> QueueItem;
	struct Cmp {
		bool operator()(const QueueItem& a, const QueueItem& b) const {
			return a.first > b.first; // min-heap
		}
	};

	// Create the search containers (automatically sort nodes by priority)
	std::priority_queue<QueueItem, std::vector<QueueItem>, Cmp> openedQueue;
	std::unordered_map<PosTime, float, PosTimeHash> bestCostAt;

	// Reservation table is useless with A*, use it with other algorithms like CBS (multi-agent)
	// std::shared_ptr<ReservationTable> rtPtr = reservationTable.lock();

	// Create start node & register it
	float heur = heuristic(agent.start, agent.goal);
	NodePtr startNode = std::make_shared<Node>(heur, 0.0f, std::weak_ptr<AStarPathfinder::Node>(), agent.start, time);
	openedQueue.emplace(startNode->getCost(), startNode);

	// Initialize goalNode
	NodePtr goalNode = nullptr;

	// Loop over opened paths
	while (!openedQueue.empty()) {
		NodePtr current = openedQueue.top().second;
		openedQueue.pop();

		// Si on est déjà venu ici à ce timing pour un coût <=, inutile de poursuivre
		float cost = current->getCost();
		PosTime key = PosTime(current->position, current->time);
		auto it = bestCostAt.find(key);
		if (it != bestCostAt.end() && it->second <= cost) continue;
		bestCostAt[key] = cost;

		// Objectif atteint
		const Position& position = current->position;
		if (position == agent.goal) {
			goalNode = current;
			break;
		}

		// Loop over move possibilities
		for (const Position& dir : dirs) {
			const Position npos = Position(position.x + dir.x, position.y + dir.y);
			int nextTime = current->time + 1;

			// Check conditions & verifications
			if (constraints.has(agent.id, npos, nextTime)) continue;
			if (!isValid(agent.id, npos, nextTime)) continue;

			// Calculate costs & heuristic
			float g = current->globalCost + actionCost;
			if (this->extraCost) g += std::fmax(this->extraCost(agent.id, npos, nextTime), 0.0f);
			float h = this->heuristic(npos, agent.goal);

			// Create new valid neighbor node
			NodePtr neighborNode = std::make_shared<Node>(h, g, current, npos, nextTime);
			openedQueue.emplace(neighborNode->getCost(), neighborNode);
		}
	}

	// Reconstituer le chemin
	AgentPath path;
	if (goalNode) {
		path.setCost(goalNode->getCost());
		NodePtr n = goalNode;
		while (n) {
			path.emplace_back(n->position, n->time);
			n = n->parent;
		} std::reverse(path.begin(), path.end());
	}

	// Move path to return argument
	return path;
}

bool AStarPathfinder::isValid(int agentId, const Position& pos, int time) const
{
	// Ensure is in settings bounds
	if (pos.x < 0 || pos.x >= width ||
		pos.y < 0 || pos.y >= height ||
		time >= MAX_TURNS
	) return false;

	// Check & invoke validate callback
	if (!this->validate) return true;
	return this->validate(agentId, pos, time);
}