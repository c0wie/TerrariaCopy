#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>


bool CollisionSolver(Tile &tile, Player &player, Vector2 &direction);
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
    std::array<Tile, TILES_COUNT> tiles{};
private:
    int substeps{4};
};
