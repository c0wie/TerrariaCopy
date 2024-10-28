#pragma once
#include "Global.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.21;
constexpr float DAMAGE_TRESHOLD_SPEED = 500.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
class Player
{
public:
    Player() = default;
    void Move(Vector2 offset) { position += offset; }
    void Draw(sf::RenderWindow &window) const;
    void Update(float deltaTime, char gameState);
    void Save();
    void Load();
    void PlaceBlock();
    Vector2 GetCoords() const;
public:
    Inventory inventory;
    Vector2 position{200.0f, 150.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{1.76f * TILE_SIZE, 2.7f * TILE_SIZE};
    float jumpHeight{TILE_SIZE * 2.0f};
    float speed{TILE_SIZE * 7.8f};
    float spaceHoldTime{0.1f};
    bool canJump{false};
    bool canPlaceBlock{true};
    float canPlaceBlockTimer{PLAYER_PLACE_BLOCK_DELAY};
    float strength{100.0f};
    int health {100};
};