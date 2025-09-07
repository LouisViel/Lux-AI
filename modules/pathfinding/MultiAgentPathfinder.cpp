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


void MultiAgentPathfinder::addStay(int agentId, const Position& position)
{
    addAgent(agentId, position, position, 0);
}

void MultiAgentPathfinder::addAgent(int agentId, const Position& start, const Position& goal, int priority)
{
	agents[agentId] = MultiAgentPathfinder::Agent(start, goal, AgentPath::invalid(start, 0), priority);
}

void MultiAgentPathfinder::removeAgent(int agentId)
{
    resetPath(agentId);
    agents.erase(agentId);
}

void MultiAgentPathfinder::clear()
{
    reservationTable->reset();
    algorithm->clearReservations();
    agents.clear();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AgentPath MultiAgentPathfinder::getPath(int agentId) const
{
	const std::map<int, MultiAgentPathfinder::Agent>::const_iterator it = agents.find(agentId);
	if (it != agents.end()) return it->second.path;
	return AgentPath::invalid(Position(), -1);
}

bool MultiAgentPathfinder::hasPath(int agentId) const
{
	return getPath(agentId).isValid();
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
    algorithm->clearReservations();
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePath(int agentId, int time)
{
    // Find wanted agent
    const std::map<int, Agent>::const_iterator it = agents.find(agentId);
    if (it == agents.end()) return;

    // Copy Agents to AgentInputs for IPathfindingAlgorithm
    std::vector<AgentInput> inputs;
    const MultiAgentPathfinder::Agent& agent = it->second;
    inputs.emplace_back(agentId, agent.start, agent.goal, agent.priority);
    
    // Call Internal compute Path with selected inputs
    algorithm->removeReservation(agent.path, *reservationTable);
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePath(std::vector<int> agentIds, int time)
{
    // Copy Agents to AgentInputs for IPathfindingAlgorithm
    std::vector<AgentInput> inputs;
    for (size_t i = 0; i < agentIds.size(); ++i) {
        int id = agentIds[i];
        const std::map<int, Agent>::const_iterator it = agents.find(id);
        if (it != agents.end()) {
            const Agent& agent = it->second;
            inputs.emplace_back(id, agent.start, agent.goal, agent.priority);
            algorithm->removeReservation(agent.path, *reservationTable);
        }
    }

    // Call Internal compute Path with selected inputs
    computePathInternal(inputs, time);
}

void MultiAgentPathfinder::computePathInternal(const std::vector<AgentInput>& inputs, int time)
{
    // Appel à l'algorithme IPathfindingAlgorithm injecté
    std::map<int, AgentPath> results = algorithm->computePaths(inputs, time, *reservationTable);
    algorithm->applyReservations(results, *reservationTable);

    // Copie back des résultats dans les agents
    for (std::map<int, AgentPath>::iterator it = results.begin(); it != results.end(); ++it) {
        const std::map<int, Agent>::iterator agentIt = agents.find(it->first);
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
    algorithm->clearReservations();

    // Clear saved agents path
    for (std::map<int, Agent>::iterator it = agents.begin(); it != agents.end(); ++it) {
        AgentPath().swap(it->second.path);
    }
}

void MultiAgentPathfinder::resetPath(int agentId)
{
    // Find wanted agent
    const std::map<int, Agent>::iterator it = agents.find(agentId);
    if (it == agents.end()) return;

    // Free reservation table
    AgentPath& agentPath = it->second.path;
    algorithm->removeReservation(agentPath, *reservationTable);

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