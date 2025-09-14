#pragma once
#include "modules/utils/make_unique.hpp"
#include "modules/utils/Event.hpp"
#include "modules/utils/WeakPtr.hpp"
#include "modules/utils/Stack.hpp"
#include "modules/utils/Function.hpp"

constexpr int MAX_TURNS = 360;

#include "lux/kit.hpp"
static std::reference_wrapper<kit::Agent> staticGame = std::ref(*(kit::Agent*)nullptr);
#define ACCESS_GAME kit::Agent& gameState = staticGame.get();