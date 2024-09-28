#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Tile
{
public:
    Tile() = default;
    Tile(Vector2 Position, tileType tileType) :
        position(Position), type(tileType),
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
    void setTileProperties(tileType Type)
    {
        const tileProperties tp = tilePropertiesMap[(int)Type];
        type = Type;
        color = tp.color;
        isSolid = tp.isSolid;
    }
public:
    Vector2 position{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    tileType type{tileType::AIR};
    sf::Color color{sf::Color::Transparent};
    bool isSolid{true};
};