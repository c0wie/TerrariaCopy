#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"

class sf::RenderWindow;

void populateMap(std::array<std::array<Tile, MAP_WIDTH>, MAP_HEIGHT> &map);

class Map
{
public:
    Map();
public:
    void Update(float deltaTime);
    void Draw(sf::RenderWindow &window) const;
private:
    std::array<Vector2, 12> FindPossibleCollisionTileCoords(Vector2 position, Vector2 size) const;
public:
    Player player;
    std::array<std::array<Tile, MAP_WIDTH>, MAP_HEIGHT> tiles{};
};
