#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

struct Tile
{
public:
    enum TileType
    {
        NONE = 0,
        GRASS,
        STONE,
        LOG,
        BORDER,
        TILE_COUNT
    };
public:
    Tile() = default;
    Tile(Vector2 Position, short TileType);
    void Draw(sf::RenderWindow &window) const;
    void SetTileProperties(short Type);
    bool isCollidable() const;
    bool isNone() const;
public:
    static std::array<std::pair<sf::Color, float>, TILE_COUNT> tilePropertiesTable;
    sf::Color color{sf::Color::Transparent};
    Vector2 position{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    float durability{0.0f};
    short type{NONE};
};