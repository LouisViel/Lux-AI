#include "HandlerHelper.hpp"

void HandlerHelper::update(std::unordered_map<lux::Position, CityHandler, PositionHash>& cityHandlers, lux::Player& player)
{
    for (auto& kv : cityHandlers) {
        kv.second.validThisTurn = false;
    }

    for (std::pair<const std::string, lux::City>& city : player.cities) {
        for (const lux::CityTile& tile : city.second.citytiles) {

            auto result = cityHandlers.emplace(std::piecewise_construct,
                std::forward_as_tuple(tile.pos),
                std::forward_as_tuple(tile.cityid));
            CityHandler& handler = result.first->second;
            handler.cityPosition = tile.pos;
            handler.validThisTurn = true;
        }
    }

    // Étape 2 : Supprimer les handlers invalides
    for (auto it = cityHandlers.begin(); it != cityHandlers.end(); ) {
        if (!it->second.validThisTurn) it = cityHandlers.erase(it);
        else ++it;
    }
}

void HandlerHelper::update(std::unordered_map<std::string, WorkerHandler>& workerHandlers, std::unordered_map<std::string, CartHandler>& cartHandlers, lux::Player& player)
{
    // Étape 1 : Parcourir toutes les unités du joueur
    for (lux::Unit& unit : player.units) {
        if (unit.isWorker()) {

            // Ajouter un WorkerHandler si manquant
            auto result = workerHandlers.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(unit.id),
                std::forward_as_tuple(unit.id)
            );
            result.first->second.validThisTurn = true;

        } else if (unit.isCart()) {

            // Ajouter un CartHandler si manquant
            auto result = cartHandlers.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(unit.id),
                std::forward_as_tuple(unit.id)
            );
            result.first->second.validThisTurn = true;
        }
    }

    // Étape 2 : Supprimer les handlers invalides
    for (auto it = workerHandlers.begin(); it != workerHandlers.end(); ) {
        if (!it->second.validThisTurn) it = workerHandlers.erase(it);
        else ++it;
    }

    // Étape 2 : Supprimer les handlers invalides
    for (auto it = cartHandlers.begin(); it != cartHandlers.end(); ) {
        if (!it->second.validThisTurn) it = cartHandlers.erase(it);
        else ++it;
    }
}