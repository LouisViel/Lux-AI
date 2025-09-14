#include "MineManager.hpp"
#include "utils.hpp"
#include "../LuxHelper.hpp"

MineManager::MineManager(const std::vector<std::vector<lux::Cell>>& map)
{
	size_t iSize = map.size();
	for (size_t i = 0; i < iSize; ++i) {
		size_t jSize = map[i].size();
		for (rsize_t j = 0; j < jSize; ++j) {

			const lux::Cell& cell = map[i][j];
			if (cell.hasResource()) {
				if (cell.resource.type == lux::ResourceType::wood) this->woodNodes.insert(cell.pos);
				else if (cell.resource.type == lux::ResourceType::coal) this->coalNodes.insert(cell.pos);
				else if (cell.resource.type == lux::ResourceType::uranium) this->uraniumNodes.insert(cell.pos);
			}
		}
	}
}

const lux::Position MineManager::getTarget(std::string agentId, const lux::Position& start, bool wood, bool coal, bool uranium)
{
    struct Candidate {
        lux::Position pos;
        double score = 0.0;
    };

    std::unordered_set<lux::Position> visited;
    std::vector<Candidate> candidates;

    auto addCandidates = [&](const std::unordered_set<lux::Position>& nodes, double value, bool unlocked) {
        if (!unlocked) return;
        for (const Position& node : nodes) {
            for (const Position& dir : LuxHelper::dirs) {
                const Position neighbor = Position(node.x + dir.x, node.y + dir.y);
                if (isValid(neighbor) && visited.insert(neighbor).second) {
                    Candidate candidate;
                    candidate.pos = neighbor;
                    candidate.score = computeScore(start, neighbor, value);
                    candidates.push_back(candidate);
                }
            }
        }
    };

    addCandidates(woodNodes, 20.0, wood);
    addCandidates(coalNodes, 50.0, coal && LuxHelper::canMineCoal());
    addCandidates(uraniumNodes, 85.0, uranium && LuxHelper::canMineUranium());

    if (candidates.empty()) {
        return Position();
    }

    // Retourne le max score
    buffer[agentId] = std::max_element(candidates.begin(), candidates.end(),
        [](const Candidate& a, const Candidate& b) { return a.score < b.score; })->pos;
    return buffer[agentId];
}

void MineManager::remove(const std::string& agentId)
{
    buffer.erase(agentId);
}

bool MineManager::isValid(const lux::Position& pos)
{
    const lux::Cell* const cell = LuxHelper::getCell(pos);
    if (!cell || !cell->hasResource()) return false;
    for (const std::pair<const std::string, lux::Position>& pair : buffer) {
        if (pair.second == pos) return false;
    } return true;
}

double MineManager::computeScore(const lux::Position& from, const lux::Position& to, double value) {
	double dx = static_cast<double>(from.x - to.x);
	double dy = static_cast<double>(from.y - to.y);
	double distance = std::sqrt(dx * dx + dy * dy); // Euclidienne, ou Manhattan si tu préfères
	return value / (1.0 + distance); // Plus proche = mieux
}