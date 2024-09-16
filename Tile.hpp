#pragma once
#include "Global.hpp"
#include <SFML/Graphics.hpp>

class Tile
{
public:
    Tile(size_t ID, sf::Vector2f position, tileType tileType) :
        position(position),
        type(tileType), isSolid(tilePropertiesMap[(int)tileType].isSolid), color(tilePropertiesMap[(int)tileType].color)
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
    size_t ID;
    sf::Vector2f position{0.0f, 0.0f};
    sf::Vector2f size{50.0f, 50.0f};
    tileType type{tileType::GRASS};
    sf::Color color{sf::Color::Green};
    bool isSolid{true};
};