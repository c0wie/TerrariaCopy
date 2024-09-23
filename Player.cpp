#include "Player.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

void Player::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape player(size);
    player.setOrigin(size / 2.0f);
    player.setPosition(position);
    player.setFillColor(sf::Color::Red);
    window.draw(player);
}

void Player::Update(float deltaTime)
{
    velocity.x = 0.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        velocity.x -= speed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += speed;
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
}