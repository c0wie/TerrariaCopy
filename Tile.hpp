#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Tile
{
public:
    Tile() = default;
    Tile(Vector2 Position, ItemType TileType) :
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
    void setTileProperties(ItemType Type)
    {
        const tileProperties tp = tilePropertiesTable[(int)Type];
        type = Type;
        color = tp.color;
        isSolid = tp.isSolid;
        canPlaceBlock = tp.canPlaceBlock;
        durability = tp.durability;
    }
public:
    Vector2 position{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    ItemType type{ItemType::NONE};
    sf::Color color{sf::Color::Transparent};
    float durability{0.0f};
    bool isSolid{false};
    bool canPlaceBlock{false};
};