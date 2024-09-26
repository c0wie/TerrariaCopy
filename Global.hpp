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

enum class tileType
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
    tileProperties(sf::Color color, bool isSolid, bool isStatic) :
        color(color), isSolid(isSolid), isStatic(isStatic) {}
    sf::Color color{sf::Color::Black};
    bool isSolid{true};
    bool isStatic{false};
};

const std::array<tileProperties, static_cast<size_t>(tileType::COUNT)> tilePropertiesMap = 
{
    tileProperties(sf::Color::White, false, true),
    tileProperties(sf::Color::Green, true, true),
    tileProperties({111, 118, 130}, true, true),
    tileProperties(sf::Color::Yellow, false, true),
    tileProperties({164, 52, 235}, true, true)
};

