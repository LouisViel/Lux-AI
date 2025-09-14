#include "LuxHelper.hpp"
#include "utils.hpp"
#include "lux/kit.hpp"

const Position LuxHelper::dirs[5] = {
	Position(1, 0), // right
	Position(-1, 0), // left
	Position(0, 1), // top
	Position(0, -1), // bottom
	Position(0, 0) // stay
};

int LuxHelper::woodRate = 0; // 1
int LuxHelper::coalRate = 0; // 10
int LuxHelper::uraniumRate = 0; // 40

int LuxHelper::dayDuration = 0; // 30 turns / 40 total turns
int LuxHelper::nightDuration = 0; // 10 turns / 40 total turns
int LuxHelper::maxRoad = 0; // 6

lux::Player* LuxHelper::player = nullptr;
lux::Player* LuxHelper::opponent = nullptr;
lux::GameMap* LuxHelper::gameMap = nullptr;
std::string LuxHelper::error = "";

std::unordered_map<std::string, int> LuxHelper::idInternal;
std::unordered_map<int, std::string> LuxHelper::idExternal;
std::vector<int> LuxHelper::freeIds;
int LuxHelper::nextId = 0;


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


void LuxHelper::init()
{
	const nlohmann::json& parameters = lux::GAME_CONSTANTS["PARAMETERS"];
	LuxHelper::dayDuration = parameters["DAY_LENGTH"];
	LuxHelper::nightDuration = parameters["NIGHT_LENGTH"];
	LuxHelper::maxRoad = parameters["MAX_ROAD"];

	const nlohmann::json& fuelRate = parameters["RESOURCE_TO_FUEL_RATE"];
	LuxHelper::woodRate = static_cast<int>(fuelRate[LuxHelper::WOOD]);
	LuxHelper::coalRate = static_cast<int>(fuelRate[LuxHelper::COAL]);
	LuxHelper::uraniumRate = static_cast<int>(fuelRate[LuxHelper::URANIUM]);
}

void LuxHelper::update(lux::Player& player, lux::Player& opponent, lux::GameMap& gameMap)
{
	LuxHelper::player = &player;
	LuxHelper::opponent = &opponent;
	LuxHelper::gameMap = &gameMap;

	// Update ids mapping
	std::vector<std::string> currentIds;

	// Update ids mapping (Player)
	for (const lux::Unit& unit : player.units) currentIds.push_back(unit.id);
	for (std::pair<const std::string, lux::City> it : player.cities) {
		for (const lux::CityTile& tile : it.second.citytiles)
			currentIds.push_back(tile.cityid);
	}

	// Update ids mapping (Opponent)
	for (const lux::Unit& unit : opponent.units) currentIds.push_back(unit.id);
	for (std::pair<const std::string, lux::City> it : opponent.cities) {
		for (const lux::CityTile& tile : it.second.citytiles)
			currentIds.push_back(tile.cityid);
	}

	// Update ids mapping
	LuxHelper::updateIds(currentIds);
}

void LuxHelper::updateIds(const std::vector<std::string>& currentIds)
{
	std::unordered_set<std::string> alive(currentIds.begin(), currentIds.end());

	// Step 1: remove IDs not alive anymore
	for (auto it = LuxHelper::idInternal.begin(); it != LuxHelper::idInternal.end();) {
		if (alive.find(it->first) == alive.end()) {
			int id = it->second;
			LuxHelper::idExternal.erase(id);
			LuxHelper::freeIds.push_back(id);
			it = LuxHelper::idInternal.erase(it);
		} else ++it;
	}

	// Step 2: ensure all alive strings have an int ID
	/*for (const std::string& s : currentIds) {
		LuxHelper::getId(s);
	}*/
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


int LuxHelper::getId(const std::string& s)
{
	auto it = LuxHelper::idInternal.find(s);
	if (it != LuxHelper::idInternal.end()) return it->second;

	int id;
	if (!LuxHelper::freeIds.empty()) {
		id = freeIds.back();
		LuxHelper::freeIds.pop_back();
	} else id = nextId++;

	// insère les deux directions
	LuxHelper::idInternal.emplace(s, id);
	LuxHelper::idExternal[id] = s;
	return id;
}

const std::string& LuxHelper::getId(const int id)
{
	auto it = LuxHelper::idExternal.find(id);
	if (it == LuxHelper::idExternal.end()) return LuxHelper::error;
	return it->second;
}

bool LuxHelper::isValid(const lux::Position& pos)
{
	return pos.x >= 0 && pos.x < LuxHelper::gameMap->width &&
		pos.y >= 0 && pos.y < LuxHelper::gameMap->height;
}

lux::Cell* LuxHelper::getCell(const lux::Position& pos)
{
	return isValid(pos) ? LuxHelper::gameMap->getCellByPos(pos) : nullptr;
}

lux::DIRECTIONS LuxHelper::getDirection(const lux::Position& pos, const lux::Position& target)
{
	if (pos == target) return lux::DIRECTIONS::CENTER;
	if (target.x == pos.x) {
		if (target.y == pos.y - 1) return lux::DIRECTIONS::NORTH;
		if (target.y == pos.y + 1) return lux::DIRECTIONS::SOUTH;
	} else if (target.y == pos.y) {
		if (target.x == pos.x + 1) return lux::DIRECTIONS::EAST;
		if (target.x == pos.x - 1) return lux::DIRECTIONS::WEST;
	} return lux::DIRECTIONS::CENTER;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


const lux::Unit* const LuxHelper::getUnit(const lux::Position& pos)
{
	std::vector<lux::Unit> unitsPlayer = LuxHelper::player->units;
	for (const lux::Unit& unit : unitsPlayer)
		if (unit.pos == pos) return &unit;
	std::vector<lux::Unit> unitsOpponent = LuxHelper::opponent->units;
	for (const lux::Unit& unit : unitsOpponent)
		if (unit.pos == pos) return &unit;
	return nullptr;
}

const lux::Unit* const LuxHelper::getUnit(std::string id)
{
	return LuxHelper::getUnit(LuxHelper::player, id);
}

const lux::Unit* const LuxHelper::getUnit(int player, std::string id)
{
	ACCESS_GAME;
	return LuxHelper::getUnit(&gameState.players[player], id);
}

const lux::Unit* const LuxHelper::getUnit(const lux::Player* const player, std::string id)
{
	std::vector<lux::Unit>::const_iterator it = std::find_if(
		player->units.begin(), player->units.end(),
		[id](const lux::Unit& unit) {
			return unit.id == id;
	});

	if (it != player->units.end()) return &*it;
	return nullptr;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool LuxHelper::surviveNight(int nightCount, int fuelCost, const lux::Cargo& cargo)
{
	// Copy ressources
	int wood = cargo.wood;
	int coal = cargo.coal;
	int uranium = cargo.uranium;
	return LuxHelper::nightCostOptimal(nightCount, fuelCost, wood, coal, uranium);
}

bool LuxHelper::nightCostOptimal(int nightCount, int fuelCost, int wood, int coal, int uranium)
{
	// Setup consume lambda helper
	auto consume = [](int& fuelNeeded, int& resource, int fuelPerUnit) {
		while (fuelNeeded > 0 && resource > 0) {
			--resource;
			fuelNeeded -= fuelPerUnit;
		}
		};

	// Check if can survive at night
	for (int i = 0; i < nightCount; ++i) {
		int fuelNeeded = fuelCost;
		consume(fuelNeeded, wood, LuxHelper::woodRate);
		consume(fuelNeeded, coal, LuxHelper::coalRate);
		consume(fuelNeeded, uranium, LuxHelper::uraniumRate);
		if (fuelNeeded > 0) return false;
	} return true;
}

bool LuxHelper::citySurviveNight(const lux::City* city)
{
	int cityCost = 0;
	for(const lux::CityTile& tile : city->citytiles)
	{
		const Position& p = tile.pos;
		int costFactor = 0;
		for (uint32_t i = 0; i < 4; i++)
		{
			const lux::CityTile* neighbour = LuxHelper::getCity(Position(p.x + dirs[i].x, p.y + dirs[i].y));
			if (neighbour && neighbour->team == tile.team)
				costFactor++;
		}
		cityCost += 23 - 5 * costFactor;
	}
	return city->fuel - cityCost > 0;
}

bool LuxHelper::isNight()
{
	ACCESS_GAME;
	int turnInCycle = gameState.turn % (LuxHelper::dayDuration + LuxHelper::nightDuration);
	return turnInCycle >= LuxHelper::dayDuration;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


const lux::City* const LuxHelper::getCity(const std::string& id)
{
	std::map<std::string, lux::City>::const_iterator itp = player->cities.find(id);
	if (itp != player->cities.end()) return &itp->second;
	std::map<std::string, lux::City>::const_iterator ito = opponent->cities.find(id);
	if (ito != opponent->cities.end()) return &ito->second;
	return nullptr;
}

const lux::CityTile* const LuxHelper::getCity(const lux::Position& pos)
{
	const lux::Cell* const cell = LuxHelper::getCell(pos);
	return cell ? cell->citytile : nullptr;
}

bool LuxHelper::isCity(const lux::Position& pos)
{
	return getCity(pos) != nullptr;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool LuxHelper::isMining(const lux::Position& pos)
{
	for (const Position& dir : LuxHelper::dirs) {
		const Position npos = Position(pos.x + dir.x, pos.y + dir.y);
		const lux::Cell* const cell = LuxHelper::getCell(npos);
		if (cell && cell->hasResource()) return true;
	} return false;
}

bool LuxHelper::isMining(const lux::Position& pos, const lux::ResourceType type)
{
	for (const Position& dir : LuxHelper::dirs) {
		const Position npos = Position(pos.x + dir.x, pos.y + dir.y);
		const lux::Cell* const cell = LuxHelper::getCell(npos);
		if (cell && cell->hasResource() && cell->resource.type == type) return true;
	} return false;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool LuxHelper::canMineCoal()
{
	return LuxHelper::player->researchedCoal();
}

bool LuxHelper::canMineCoal(int player)
{
	ACCESS_GAME;
	return gameState.players[player].researchedCoal();
}

bool LuxHelper::canMineUranium()
{
	return LuxHelper::player->researchedUranium();
}

bool LuxHelper::canMineUranium(int player)
{
	ACCESS_GAME;
	return gameState.players[player].researchedUranium();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


float LuxHelper::getRoad(const lux::Position& pos)
{
	const lux::Cell* const cell = LuxHelper::getCell(pos);
	return cell ? cell->road : 0.0f;
}

bool LuxHelper::isRoad(const lux::Position& pos)
{
	return LuxHelper::getRoad(pos) > 0.0f;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


bool LuxHelper::isBlocked(const lux::Position& pos)
{
	ACCESS_GAME;
	int pTeam = player->team;
	for (const Position& dir : LuxHelper::dirs) {
		const Position npos = Position(pos.x + dir.x, pos.y + dir.y);
		
		// Get cell at pos
		const lux::Cell* const cell = LuxHelper::getCell(npos);
		if (!cell) continue;

		// Check if ennemy city is blocking
		bool isCityBlocked = false;
		if (cell->citytile) {
			int team = cell->citytile->team;
			isCityBlocked = team != pTeam;
		}

		// Check if unit is blocking
		if (!isCityBlocked) {
			const lux::Unit* const unit = LuxHelper::getUnit(npos);
			if (!unit) return false;
		}

	// We are blocked
	} return true;
}
