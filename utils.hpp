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

class MoveManager;
static std::reference_wrapper<MoveManager> staticMoveManager = std::ref(*(MoveManager*)nullptr);
#define ACCESS_MOVE MoveManager& moveManager = staticMoveManager.get();

class MineManager;
static std::reference_wrapper<MineManager> staticMineManager = std::ref(*(MineManager*)nullptr);
#define ACCESS_MINE MineManager& mineManager = staticMineManager.get();

static std::reference_wrapper<std::vector<std::string>> staticActions = std::ref(*(std::vector<std::string>*)nullptr);
#define ACCESS_OUTPUT std::vector<std::string>& luxOutput = staticActions.get();