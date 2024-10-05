#pragma once
#include <array>
#include <SFML/Graphics.hpp>


constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
constexpr int TILES_COUNT = 100;
constexpr float TILE_SIZE = 50.0f;
constexpr int MAP_WIDTH = 100;
constexpr int MAP_HEIGHT = 30;
constexpr int INVENTORY_COUNT = 40; 
constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.17;

enum class TileType
{
    AIR,
    GRASS,
    STONE,
    LOG,
    BORDER,
    COUNT
};

struct tileProperties
{
    tileProperties(sf::Color color, bool isSolid, bool canPlaceBlock, float durability) :
        color(color), isSolid(isSolid), canPlaceBlock(canPlaceBlock), durability(durability) {}
    sf::Color color{sf::Color::Black};
    bool isSolid{true};
    bool canPlaceBlock{false};
    float durability{0.0f};
};

const std::array<tileProperties, static_cast<size_t>(TileType::COUNT)> tilePropertiesMap = 
{
    tileProperties(sf::Color::White, false, false, 0.0f),
    tileProperties(sf::Color::Green, true, true, 100),
    tileProperties({111, 118, 130}, true, true, 300),
    tileProperties(sf::Color::Yellow, false, true, 150),
    tileProperties({164, 52, 235}, true, true, 10000.0f)
};

