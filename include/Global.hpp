#pragma once
#include "SFML/Graphics.hpp"
#include "Vector2.hpp"

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
constexpr float TILE_SIZE = 16.0f;
constexpr int MAP_WIDTH = 200;
constexpr int MAP_HEIGHT = 50;
constexpr int INVENTORY_COUNT = 40; 
constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.21;
constexpr char TOP_INTERSECTION = 0b1000;
constexpr char LEFT_INTERSECTION = 0b0100;
constexpr char BOTTOM_INTERSECTION = 0b0010;
constexpr char RIGHT_INTERSECTION = 0b0001;

constexpr std::array<Vector2, 9> texturesCountInFile = 
{
    Vector2(16, 15), Vector2(16, 22), Vector2(64, 12), Vector2(), Vector2(3, 1), Vector2(16, 15),
    Vector2(16, 15), Vector2(16, 15), Vector2(16, 15)
};

