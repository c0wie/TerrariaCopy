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
    bool canJump{false};
    bool isSprinting{false};
};