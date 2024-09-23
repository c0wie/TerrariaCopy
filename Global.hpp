#pragma once
#include <array>
#include <SFML/Graphics.hpp>


const std::array<std::string, 10> mapSketch = 
{
    "l0l0000000000000000",
    "l0l000000000gg00000",
    "l0l00000000gggg0000",
    "gggg000000gggggg000",
    "sssssssssssssssssss"
};

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
constexpr int TILES_COUNT = 100;
constexpr float TILE_SIZE = 50.0f;
constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 5;

enum class tileType
{
    AIR,
    GRASS,
    STONE,
    LOG,
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
    tileProperties(sf::Color::Yellow, false, true)
};

