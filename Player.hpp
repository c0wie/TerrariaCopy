#pragma once

#include "Global.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

class Player
{
public:
    Player() = default;
    void Move(sf::Vector2f offset) { position += offset; }
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape player(size);
        player.setOrigin(size / 2.0f);
        player.setPosition(position);
        player.setFillColor(sf::Color::Red);
        window.draw(player);
    }
    void Update(float deltaTime)
    {
        velocity.x = 0.0f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            velocity.x = -speed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            velocity.x = speed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canJump)
        {
            if(spaceHoldTime < MAX_SPACE_HOLD_TIME)
            {
                spaceHoldTime += deltaTime;
            }
            else
            {
                canJump = false;
            }
            velocity.y = -sqrtf(2.0f * GRAVITY * jumpHeight * spaceHoldTime / MAX_SPACE_HOLD_TIME);
        }
        else 
        {
            spaceHoldTime = 0.0f;
            canJump = false;
        }
        velocity.y += GRAVITY * deltaTime;
        Move(velocity * deltaTime);
    }
    void OnCollision(sf::Vector2f direction)
    {
        if(direction.x != 0.0f)
        {
            velocity.x = 0.0f;
        }
        if(direction.y > 0.0f)
        {
            velocity.y = 0.0f;
            canJump = true;
        }
        else if(direction.y < 0.0f)
        {
            velocity.y = 0.0f;
        }
    }
public:
    sf::Vector2f position{0.0f, -100.0f};
    sf::Vector2f velocity{0.0f, 0.0f};
    sf::Vector2f size{50.0f, 100.0f};
    float jumpHeight{100.0f};
    float speed{200.0f};
    float spaceHoldTime{0.1f};
    bool canJump{false};
    bool isSprinting{false};
};