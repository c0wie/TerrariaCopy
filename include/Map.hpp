#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Map
{
public:
    Map();
public:
    void Draw(Vector2 playerPosition, sf::RenderWindow &window);
    void Generate();
    void Save();
    void Load();
    void HandleCollisions(Player &player, float deltaTime);
    Tile &SafeGetTile(Vector2 coords);
    Tile &UnsafeGetTile(Vector2 coords);
    const Tile &SafeGetTile(Vector2 coords) const;
    const Tile &UnsafeGetTile(Vector2 coords) const;
    std::vector<Vector2> GetCollidableTilesCoords(Vector2 playerPosition, Vector2 playerSize) const;
    std::vector<Vector2> GetBreakableTilesCoords(Vector2 playerPosition, Vector2 playerSize) const;
    std::vector<Vector2> GetVisibleTilesCoords(Vector2 playerPosition) const;
    std::vector<Vector2> GetPlayerBBTilesCoords(Vector2 playerPosition) const;
    std::vector<Vector2> GetTreeTilesCoords(Vector2 treeTileCoords) const;
    void UpdateSurroundingTiles(Vector2 centerTileCoords);
    // Resets light state in whole map then updates it
    void UpdateLighting();
    void PlaceOrePatch(Vector2 tileCoords, short oreType, float spawnChance, int &succesfulPositions);
    Tile *Raycast(Vector2 startPosition, Vector2 targetPosition);
    short CheckTileIntersection(Vector2 coords);
    bool IsValidCoords(Vector2 coords) const;
public:
    std::vector<Tile> tiles;
    std::vector<Vector2> lightSources; 
    sf::Font font;
};

/*
    Returns a vector of tile coordinates within the grid, sorted in ascending order.
    The first element (vector[0]) contains {smallestX, smallestY}.
    The last element (vector[vector.size() - 1]) contains {biggestX, biggestY}.
*/
std::vector<Vector2>GetTileCoordsInArea(const std::vector<Tile> &map, Vector2 areaCenter, Vector2 areaSize);