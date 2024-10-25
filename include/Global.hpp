#pragma once
#include "SFML/Graphics.hpp"
#include "Vector2.hpp"

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float TILE_SIZE = 16.0f;
constexpr int MAP_WIDTH = 200;
constexpr int MAP_HEIGHT = 50;
constexpr char GS_MAP = 0;
constexpr char GS_INVENTORY = 1;

constexpr std::array<Vector2, 8> texturesCountInFile = 
{
    Vector2(16, 15), Vector2(16, 22), Vector2(64, 12), Vector2(16, 15),
    Vector2(16, 15), Vector2(16, 15), Vector2(16, 15), Vector2(3, 1)
};

