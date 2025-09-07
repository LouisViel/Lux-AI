#pragma once
#include "utils.hpp"

#include "lux/position.hpp"
using Position = lux::Position;
using FuncPosition = std::function<Position()>;

class AgentBelief;
using BeliefMap = std::unordered_map<std::string, std::shared_ptr<AgentBelief>>;
using BeliefMapPtr = std::weak_ptr<BeliefMap>;