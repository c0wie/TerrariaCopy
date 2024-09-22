#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Tile
{
public:
    Tile() = default;
    Tile(size_t ID, Vector2 Position, tileType tileType) :
        ID(ID), position(Position), type(tileType),
        isSolid(tilePropertiesMap[(int)tileType].isSolid),
        color(tilePropertiesMap[(int)tileType].color)
    {}
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape tile(size);
        tile.setOrigin(size / 2.0f);
        tile.setPosition(position);
        tile.setFillColor(color);
        window.draw(tile);
    }
public:
    size_t ID{0};
    Vector2 position{0.0f, 0.0f};
    Vector2 size{50.0f, 50.0f};
    tileType type{tileType::GRASS};
    sf::Color color{sf::Color::Green};
    bool isSolid{true};
};