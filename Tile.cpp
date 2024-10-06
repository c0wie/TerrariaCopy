#include "Tile.hpp"
#include "Math.hpp"


Tile::Tile(Vector2 Position, short TileType) :
    position(Position),
    type(TileType)
{
    SetTileProperties(type);
}

void Tile::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape tile(size);
    tile.setOrigin(size / 2.0f);
    tile.setPosition(position);
    tile.setFillColor(color);
    window.draw(tile);
}

void Tile::SetTileProperties(short Type)
{
    auto tp = tilePropertiesTable[Type];
    type = Type;
    color = tp.first;
    durability = tp.second;
}

bool Tile::isCollidable() const
{
    return type != NONE && type != LOG;
}

bool Tile::isNone() const
{
    return type == NONE;
}

std::array<std::pair<sf::Color, float>, Tile::TileType::TILE_COUNT> Tile::tilePropertiesTable =
{
    std::pair(sf::Color::Transparent, 0.0f),
    std::pair(sf::Color::Green, 100.0f),
    std::pair(sf::Color{111, 118, 130}, 300.0f),
    std::pair(sf::Color::Yellow, 150.0f),
    std::pair(sf::Color{164, 52, 235}, INF),
};