#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Tile
{
public:
    Tile() = default;
    Tile(Vector2 Position, TileType TileType) :
        position(Position),
        type(TileType)
    {
        setTileProperties(type);
    }
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape tile(size);
        tile.setOrigin(size / 2.0f);
        tile.setPosition(position);
        tile.setFillColor(color);
        window.draw(tile);
    }
    void setTileProperties(TileType Type)
    {
        const tileProperties tp = tilePropertiesMap[(int)Type];
        type = Type;
        color = tp.color;
        isSolid = tp.isSolid;
        canPlaceBlock = tp.canPlaceBlock;
    }
public:
    Vector2 position{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    TileType type{TileType::AIR};
    sf::Color color{sf::Color::Transparent};
    bool isSolid{false};
    bool canPlaceBlock{false};
};