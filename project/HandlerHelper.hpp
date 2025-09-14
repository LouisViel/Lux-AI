#pragma once
#include <unordered_map>
#include <string>
#include "handlers/handlers/CityHandler.hpp"
#include "handlers/units/WorkerHandler.hpp"
#include "handlers/units/CartHandler.hpp"

class HandlerHelper
{
public:
	HandlerHelper() = delete;
	~HandlerHelper() = delete;

	static void update(std::unordered_map<lux::Position, CityHandler, PositionHash>& cityHandlers, lux::Player& player);
	static void update(std::unordered_map<std::string, WorkerHandler>& workerHandlers, lux::Player& player);
	static void update(std::unordered_map<std::string, CartHandler>& cartHandlers, lux::Player& player);
};