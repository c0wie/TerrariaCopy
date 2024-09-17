#pragma once
#include <array>
#include <SFML/Graphics.hpp>

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.3f;

enum class tileType
{
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
    tileProperties(sf::Color::Green, true, true),
    tileProperties(sf::Color::Black, true, true),
    tileProperties(sf::Color::Yellow, false, true)
};

