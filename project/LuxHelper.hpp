#pragma once
#include "lux/kit.hpp"
using Position = lux::Position;

class LuxHelper
{
public:
	static constexpr const char* WOOD = "WOOD";
	static constexpr const char* COAL = "COAL";
	static constexpr const char* URANIUM = "URANIUM";

	static const lux::Position dirs[5];

public:
	static int woodRate; // 1
	static int coalRate; // 10
	static int uraniumRate; // 40
	
	static int dayDuration; // 30 turns / 40 total turns
	static int nightDuration; // 10 turns / 40 total turns
	static int maxRoad; // 6

	static lux::Player* player;
	static lux::Player* opponent;
	static lux::GameMap* gameMap;
	static std::string error;

private:
	static std::unordered_map<std::string, int> idInternal;
	static std::unordered_map<int, std::string> idExternal;
	static std::vector<int> freeIds;
	static int nextId;

public:
	LuxHelper() = delete;
	LuxHelper(const LuxHelper&) = delete;
	LuxHelper& operator=(const LuxHelper&) = delete;
	~LuxHelper() = delete;

	static void init();
	static void update(lux::Player& player, lux::Player& opponent, lux::GameMap& gameMap);
	static void updateIds(const std::vector<std::string>& currentIds);
	
public:
	static int getId(const std::string& id);
	static const std::string& getId(const int id);

	static bool isValid(const lux::Position& pos);
	static lux::Cell* getCell(const lux::Position& pos);

public:
	static const lux::Unit* const getUnit(const lux::Position& pos);
	static const lux::Unit* const getUnit(std::string id);
	static const lux::Unit* const getUnit(int player, std::string id);
	static const lux::Unit* const getUnit(const lux::Player* const player, std::string id);

	static bool surviveNight(int nightCount, int fuelCost, const lux::Cargo& cargo);
	static bool nightCostOptimal(int nightCount, int fuelCost, int wood = 0, int coal = 0, int uranium = 0);
	static bool isNight();

	static const lux::City* const getCity(const std::string& id);
	static const lux::CityTile* const getCity(const lux::Position& pos);
	static bool isCity(const lux::Position& pos);

	static bool isMining(const lux::Position& pos);
	static bool isMining(const lux::Position& pos, const lux::ResourceType type);

	static bool canMineCoal();
	static bool canMineCoal(int player);
	static bool canMineUranium();
	static bool canMineUranium(int player);

	static float getRoad(const lux::Position& pos);
	static bool isRoad(const lux::Position& pos);

	static bool isBlocked(const lux::Position& pos);
};