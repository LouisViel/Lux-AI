#include "utils.hpp"

#include "lux/position.hpp"
using Position = lux::Position;

using Action = std::function<void()>;
using FuncPosition = std::function<Position()>;
using FuncBool = std::function<bool()>;

class AgentBelief;
using BeliefMap = std::map<std::string, std::shared_ptr<AgentBelief>>;
using BeliefMapPtr = std::weak_ptr<BeliefMap>;