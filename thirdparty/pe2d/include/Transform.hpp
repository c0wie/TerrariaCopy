#pragma once

#include "Vector2.hpp"
#include "Math.hpp"

namespace pe2d
{
    /* 
        Struct holding basic information about object like:
        - position
        - scale
        - rotation in Deegres
    */
    struct Transform
    {
    public:
        constexpr Transform() = default;
        constexpr Transform(Vector2 position, Vector2 scale, float angleRadians) : 
            position(position), scale(scale), rotation(angleRadians) {}
    public:
        constexpr void Move(Vector2 offset) { position += offset; }
        constexpr void Rotate(float angleRadians) { rotation += angleRadians; }
    public:
        Vector2 position{0.0f, 0.0f};
        Vector2 scale{1.0f, 1.0f};
        float rotation{0.0f};
    };
}