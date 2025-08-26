#include "utils.hpp"

#include "lux/position.hpp"
using Position = lux::Position;

using Action = std::function<void()>;
using FuncPosition = std::function<Position()>;
using FuncBool = std::function<bool()>;