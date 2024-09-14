#pragma once
#include "Global.hpp"
#include <SFML/Graphics.hpp>

struct Tile
{
public:
    Tile() = default;
    Tile(sf::Vector2f position, tileType tileType);
public:
    void Draw(sf::RenderWindow &window) const;
public:
    sf::Vector2f position{0.0f, 0.0f};
    sf::Vector2f size{50.0f, 50.0f};
    tileType type{tileType::GRASS};
    sf::Color color{sf::Color::Green};
    bool isSolid{true};
};