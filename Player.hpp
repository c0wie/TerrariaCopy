#pragma once

#include "Global.hpp"
#include "RigidObject.hpp"
#include <SFML/Graphics.hpp>

class Player : public pe2d::RigidObject
{
public:
    Player() :
        pe2d::RigidObject(2137U, {50.0f, 100.0f}, pe2d::Transform({SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {1.0f, 1.0f}, 0.0f),
                            100.0f, {0.0f, GRAVITY}, false)
    {}
public:
    constexpr float GetSpeed() const { return m_Speed; } 
    constexpr float GetJumpHeight() const { return m_JumpHeight; }
    constexpr bool CanJump() const { return m_CanJump; }
    constexpr bool IsSprinting() const { return m_IsSprinting; }

    constexpr void SetSpeed(float speed) { m_Speed = speed; }
    constexpr void SetJumpHeight(float jumpHeight) { m_JumpHeight = jumpHeight; }
    constexpr void SetCanJump(bool canJump) { m_CanJump = canJump; }
    constexpr void SetSprintState(bool isSprinting) {m_IsSprinting = isSprinting; }  
private:
    float m_JumpHeight{100.0f};
    float m_Speed{200.0f};
    bool m_CanJump{false};
    bool m_IsSprinting{false};
};