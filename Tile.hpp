#pragma once
#include "Global.hpp"
#include "RigidObject.hpp"
#include <SFML/Graphics.hpp>

class Tile : public pe2d::RigidObject
{
public:
    Tile(size_t ID, pe2d::Vector2 position, tileType tileType) :
        pe2d::RigidObject(ID, {50.0f, 50.0f}, pe2d::Transform(position, {1.0f, 1.0f}, 0.0f), 10.0f, {0.0f, GRAVITY},
                            tilePropertiesMap[(int)tileType].isStatic),
        type(tileType), isSolid(tilePropertiesMap[(int)tileType].isSolid), color(tilePropertiesMap[(int)tileType].color)
    {}
public:
    tileType type{tileType::GRASS};
    sf::Color color{sf::Color::Green};
    bool isSolid{true};
};