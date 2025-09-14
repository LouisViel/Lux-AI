#include "HandlerHelper.hpp"

void HandlerHelper::update(std::unordered_map<lux::Position, CityHandler, PositionHash>& cityHandlers, lux::Player& player)
{
    std::unordered_set<lux::Position, PositionHash> validPositions;
    for (std::pair<const std::string, lux::City>& city : player.cities) {
        for (const lux::CityTile& tile : city.second.citytiles) {
            validPositions.insert(tile.pos);

            // Ajouter si manquant
            if (cityHandlers.find(tile.pos) == cityHandlers.end()) {
                cityHandlers.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(tile.pos),
                    std::forward_as_tuple(tile.cityid)
                );
                cityHandlers.at(tile.pos).cityPosition = tile.pos;
            }
        }
    }

    // Étape 2 : Supprimer les handlers qui ne sont plus valides
    for (auto it = cityHandlers.begin(); it != cityHandlers.end(); /* Nothing */) {
        if (validPositions.find(it->first) == validPositions.end()) {
            it = cityHandlers.erase(it);
        } else ++it;
    }
}

void HandlerHelper::update(std::unordered_map<std::string, WorkerHandler>& workerHandlers, lux::Player& player)
{
    std::unordered_set<std::string> validIds;

    // Étape 1 : Parcourir toutes les unités du joueur
    for (lux::Unit& unit : player.units) {
        if (unit.isWorker()) {
            validIds.insert(unit.id);

            // Ajouter un WorkerHandler si manquant
            if (workerHandlers.find(unit.id) == workerHandlers.end()) {
                workerHandlers.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(unit.id),
                    std::forward_as_tuple(unit.id)
                );
            }
        }
    }

    // Étape 2 : Supprimer les handlers qui ne sont plus valides
    for (auto it = workerHandlers.begin(); it != workerHandlers.end(); /* Nothing */) {
        if (validIds.find(it->first) == validIds.end()) {
            it = workerHandlers.erase(it);
        } else ++it;
    }
}

void HandlerHelper::update(std::unordered_map<std::string, CartHandler>& cartHandlers, lux::Player& player)
{
    std::unordered_set<std::string> validIds;

    // Étape 1 : Parcourir toutes les unités du joueur
    for (lux::Unit& unit : player.units) {
        if (unit.isCart()) {
            validIds.insert(unit.id);

            // Ajouter un WorkerHandler si manquant
            if (cartHandlers.find(unit.id) == cartHandlers.end()) {
                cartHandlers.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(unit.id),
                    std::forward_as_tuple(unit.id)
                );
            }
        }
    }

    // Étape 2 : Supprimer les handlers qui ne sont plus valides
    for (auto it = cartHandlers.begin(); it != cartHandlers.end(); /* Nothing */) {
        if (validIds.find(it->first) == validIds.end()) {
            it = cartHandlers.erase(it);
        } else ++it;
    }
}