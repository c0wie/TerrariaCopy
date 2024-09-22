#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>


void populateMap(std::array<Tile, TILES_COUNT> &map, int rows);

class Map
{
public:
    Map();
public:
    void Update(float deltaTime);
    void Draw(sf::RenderWindow &window) const;
public:
    Player player;
    std::array<std::array<Tile, MAP_WIDTH>, MAP_HEIGHT> tiles{};
private:
    int substeps{4};
};
