#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player() = default;
    void Move(Vector2 offset) { position += offset; }
    void Draw(sf::RenderWindow &window) const;
    void Update(float deltaTime);
public:
    Vector2 position{100.0f, 50.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, 2 * TILE_SIZE};
    float jumpHeight{100.0f};
    float speed{200.0f};
    float spaceHoldTime{0.1f};
    bool canJump{false};
    bool isSprinting{false};
};