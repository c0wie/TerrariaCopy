#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

struct Tile
{
public:
    enum Type
    {
        NONE = 0,
        STONE,
        GRASS,
        LOG,
        IRON,
        COPPER,
        GOLD,
        SILVER,
        TREETOP,
        BORDER,
        TILE_COUNT
    };
public:
    Tile() = default;
    Tile(Vector2 Position, short TileType);
    void Draw(sf::RenderWindow &window) const;
    void LoadTexture();
    // loads tile info from line from save file
    void Load(std::string &line);
    // returns info about tile in string
    std::string GetInfo() const;
    Vector2 GetCoords() const;
    void UpdateTextureRect(short intersectionInfo);
    void SetTileProperties(short Type);
    bool isCollidable() const;
    bool isNone() const;
    bool HasTexture() const;
public:
    static std::array<std::pair<sf::Color, float>, TILE_COUNT> tilePropertiesTable;
    sf::Color color{sf::Color::Transparent};
    Vector2 position{-1.0f, -1.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    float durability{0.0f};
    short type{NONE};
    Vector2 subtype{0, 0};
    std::shared_ptr<sf::Texture> txt{std::make_shared<sf::Texture>()};
};