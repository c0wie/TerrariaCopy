#pragma once

#include "Global.hpp"
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player() = default;
public:
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape playerBody{m_Size};
        playerBody.setPosition(m_Position);
        playerBody.setOrigin(m_Size / 2.0f);
        playerBody.setFillColor(sf::Color::Red);
        window.draw(playerBody);
    }
    sf::Vector2f GetPosition() const { return m_Position; }
    sf::Vector2f GetSize() const { return m_Size; }
    sf::Vector2f GetVelocity() const { return m_Velocity; }
    float GetJumpHeight() const { return m_JumpHeight; }
    float GetSpeed() const 
    {
        if(m_IsSprinting)
        {
            return m_Speed * 2.0f;
        } 
        return m_Speed; 
    }
    bool GetSprintState() const { return m_IsSprinting; }
    bool GetCanJump() const { return m_CanJump; }

    void Move(sf::Vector2f offset) { m_Position += offset; }
    void SetPostion(sf::Vector2f position) { m_Position = position; }
    void SetSize(sf::Vector2f size) { m_Size = size; }
    void AddVelocity(sf::Vector2f deltaVelocity) { m_Velocity += deltaVelocity; }
    void SetVelocity(sf::Vector2f velocity) { m_Velocity = velocity; }
    void SetJumpHeight(float jumpHeight) { m_JumpHeight = jumpHeight; }
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetCanJump(bool canJump) { m_CanJump = canJump; }
    void SetSprintState(bool isSprinting) { m_IsSprinting = isSprinting; }
private:
    sf::Vector2f m_Position {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    sf::Vector2f m_Size {50.0f, 100.0f};
    sf::Vector2f m_Velocity{0.0f, 0.0f};
    float m_JumpHeight{100.0f};
    float m_Speed{200.0f};
    bool m_CanJump{false};
    bool m_IsSprinting{false};
};