#pragma once
#include <stack>

template <typename T>
using Stack = std::stack<T, std::deque<T>>;