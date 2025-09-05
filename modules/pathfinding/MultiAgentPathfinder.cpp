#include "MultiAgentPathfinder.hpp"
#include "ReservationTable.hpp"


MultiAgentPathfinder::MultiAgentPathfinder(int width, int height, std::shared_ptr<IPathfindingMultiAlgorithm> algorithm)
	: reservationTable(make_unique<ReservationTable>(width, height)), algorithm(algorithm)
{ }

MultiAgentPathfinder::~MultiAgentPathfinder()
{
	// Empty destructor
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void MultiAgentPathfinder::addAgent(int agentId, const Position& start, const Position& goal, int priority)
{
	agents[agentId] = MultiAgentPathfinder::Agent(start, goal, AgentPath(), priority);
}

void MultiAgentPathfinder::removeAgent(int agentId)
{
    resetPath(agentId);
    agents.erase(agentId);
}

void MultiAgentPathfinder::clear()
{
    reservationTable->reset();
    agents.clear();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AgentPath MultiAgentPathfinder::getPath(int agentId) const
{
	std::map<int, MultiAgentPathfinder::Agent>::const_iterator it = agents.find(agentId);
	if (it != agents.end()) return it->second.path;
	return AgentPath();
}

bool MultiAgentPathfinder::hasPath(int agentId) const
{
	return getPath(agentId).size() > 0;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void MultiAgentPathfinder::computePaths(int time)
{
    // Copy Agents to AgentInputs for IPathfindingAlgorithm
    std::vector<AgentInput> inputs;
    for (std::map<int, Agent>::const_iterator it = agents.begin(); it != agents.end(); ++it) {
        const Agent& agent = it->second;
        inputs.push_back(AgentInput(it->first, agent.start, agent.goal, agent.priority));
    }

    // Call Internal compute Path with selected inputs
    reservationTable->reset();
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePath(int agentId, int time)
{
    // Find wanted agent
    std::map<int, Agent>::const_iterator it = agents.find(agentId);
    if (it == agents.end()) return;

    // Copy Agents to AgentInputs for IPathfindingAlgorithm
    std::vector<AgentInput> inputs;
    const MultiAgentPathfinder::Agent& agent = it->second;
    inputs.push_back(AgentInput(agentId, agent.start, agent.goal, agent.priority));

    // Call Internal compute Path with selected inputs
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePath(std::vector<int> agentIds, int time)
{
    // Copy Agents to AgentInputs for IPathfindingAlgorithm
    std::vector<AgentInput> inputs;
    for (size_t i = 0; i < agentIds.size(); ++i) {
        int id = agentIds[i];
        std::map<int, Agent>::const_iterator it = agents.find(id);
        if (it != agents.end()) {
            const Agent& agent = it->second;
            inputs.push_back(AgentInput(id, agent.start, agent.goal, agent.priority));
        }
    }

    // Call Internal compute Path with selected inputs
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePathInternal(const std::vector<AgentInput>& inputs, int time)
{
    // Appel à l'algorithme IPathfindingAlgorithm injecté
    std::map<int, AgentPath> results = std::move(algorithm->computePaths(inputs, time, *reservationTable));

    // Copie back des résultats dans les agents
    for (std::map<int, AgentPath>::iterator it = results.begin(); it != results.end(); ++it) {
        std::map<int, Agent>::iterator agentIt = agents.find(it->first);
        if (agentIt != agents.end()) agentIt->second.path = std::move(it->second);
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void MultiAgentPathfinder::resetPaths()
{
    // Reset reservation table
    reservationTable->reset();

    // Clear saved agents path
    for (std::map<int, Agent>::iterator it = agents.begin(); it != agents.end(); ++it) {
        AgentPath().swap(it->second.path);
    }
}

void MultiAgentPathfinder::resetPath(int agentId)
{
    // Find wanted agent
    std::map<int, Agent>::iterator it = agents.find(agentId);
    if (it == agents.end()) return;

    // Free reservation table
    AgentPath& agentPath = it->second.path;
    for (const std::pair<Position, int>& path : agentPath)
        reservationTable->free(path.first, path.second);

    // Clear saved agent path
    AgentPath().swap(agentPath);
}

void MultiAgentPathfinder::resetPath(std::vector<int> agentIds)
{
    // Loop over ids & call individual reset
    for (int agentId : agentIds) {
        resetPath(agentId);
    }
}