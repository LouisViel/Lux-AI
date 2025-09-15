#include "lux/kit.hpp"
#include "lux/define.cpp"
#include <string.h>
#include <vector>
#include <set>
#include <stdio.h>

#include "utils.hpp"
#include "project/LuxHelper.hpp"
#include "project/handlers/MoveManager.hpp"
#include "project/handlers/MineManager.hpp"
#include "project/HandlerHelper.hpp"


using namespace std;
using namespace lux;
int main()
{
  kit::Agent gameState = kit::Agent();
  staticGame = std::reference_wrapper<kit::Agent>(gameState);

  // initialize
  gameState.initialize();
  LuxHelper::init();

  // Initialize global managers
  MoveManager moveManager(gameState.mapWidth, gameState.mapHeight);
  MineManager mineManager(gameState.map.map);
  staticMoveManager = std::reference_wrapper<MoveManager>(moveManager);
  staticMineManager = std::reference_wrapper<MineManager>(mineManager);

  // Initialize actions buffer with global access
  vector<string> actions = vector<string>();
  staticActions = std::reference_wrapper<std::vector<std::string>>(actions);

  // Declare / Initialize handlers containers
  std::unordered_map<lux::Position, CityHandler, PositionHash> cityHandlers;
  std::unordered_map<std::string, WorkerHandler> workerHandlers;
  std::unordered_map<std::string, CartHandler> cartHandlers;

  cityHandlers.reserve(200);
  workerHandlers.reserve(200);
  cartHandlers.reserve(200);

  while (true)
  {
    /** Do not edit! **/
    // wait for updates
    gameState.update();
    //actions.clear();
    
    /** AI Code Goes Below! **/
    Player &player = gameState.players[gameState.id];
    Player &opponent = gameState.players[(gameState.id + 1) % 2];
    GameMap &gameMap = gameState.map;
    int turnId = gameState.turn;

    // Update Alive global states & brain handlers
    LuxHelper::update(player, opponent, gameMap);
    HandlerHelper::update(cityHandlers, player);
    HandlerHelper::update(workerHandlers, cartHandlers, player);

    // Call Handlers updates (calling through GOAP & pathfinding, ect.. logic)
    for (auto it = cityHandlers.begin(); it != cityHandlers.end(); ++it) it->second.update(turnId);
    for (auto it = workerHandlers.begin(); it != workerHandlers.end(); ++it) it->second.update(turnId);
    for (auto it = cartHandlers.begin(); it != cartHandlers.end(); ++it) it->second.update(turnId);

    // Original example source code :
    //vector<Cell *> resourceTiles = vector<Cell *>();
    //for (int y = 0; y < gameMap.height; y++)
    //{
    //  for (int x = 0; x < gameMap.width; x++)
    //  {
    //    Cell *cell = gameMap.getCell(x, y);
    //    if (cell->hasResource())
    //    {
    //      resourceTiles.push_back(cell);
    //    }
    //  }
    //}

    //// we iterate over all our units and do something with them
    //for (int i = 0; i < player.units.size(); i++)
    //{
    //  Unit unit = player.units[i];
    //  if (unit.isWorker() && unit.canAct())
    //  {
    //    if (unit.getCargoSpaceLeft() > 0)
    //    {
    //      // if the unit is a worker and we have space in cargo, lets find the nearest resource tile and try to mine it
    //      Cell *closestResourceTile = nullptr;
    //      float closestDist = 9999999;
    //      for (auto it = resourceTiles.begin(); it != resourceTiles.end(); it++)
    //      {
    //        auto cell = *it;
    //        if (cell->resource.type == ResourceType::coal && !player.researchedCoal()) continue;
    //        if (cell->resource.type == ResourceType::uranium && !player.researchedUranium()) continue;
    //        float dist = cell->pos.distanceTo(unit.pos);
    //        if (dist < closestDist)
    //        {
    //          closestDist = dist;
    //          closestResourceTile = cell;
    //        }
    //      }
    //      if (closestResourceTile != nullptr)
    //      {
    //        auto dir = unit.pos.directionTo(closestResourceTile->pos);
    //        actions.push_back(unit.move(dir));
    //      }
    //    }
    //    else
    //    {
    //      // if unit is a worker and there is no cargo space left, and we have cities, lets return to them
    //      if (player.cities.size() > 0)
    //      {
    //        auto city_iter = player.cities.begin();
    //        auto &city = city_iter->second;

    //        float closestDist = 999999;
    //        CityTile *closestCityTile = nullptr;
    //        for (auto &citytile : city.citytiles)
    //        {
    //          float dist = citytile.pos.distanceTo(unit.pos);
    //          if (dist < closestDist)
    //          {
    //            closestCityTile = &citytile;
    //            closestDist = dist;
    //          }
    //        }
    //        if (closestCityTile != nullptr)
    //        {
    //          auto dir = unit.pos.directionTo(closestCityTile->pos);
    //          actions.push_back(unit.move(dir));
    //        }
    //      }
    //    }
    //  }
    //}

    // you can add debug annotations using the methods of the Annotate class.
    // actions.push_back(Annotate::circle(0, 0));

    /** AI Code Goes Above! **/

    /** Do not edit! **/
    for (int i = 0; i < actions.size(); i++)
    {
      if (i != 0)
        cout << ",";
      cout << actions[i];
    }
    cout << endl;
    // end turn
    gameState.end_turn();
  }

  return 0;
}
