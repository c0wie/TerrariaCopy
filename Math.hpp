#pragma once
#include "Vector2.hpp"
#include <cmath>

constexpr float INF = std::numeric_limits<float>::infinity();
constexpr float MIN = std::numeric_limits<float>::min();
constexpr float PI = 3.1415927;
constexpr float Length(Vector2 a)
{
    return std::sqrt(a.x * a.x + a.y * a.y);
}
constexpr float SquaredLength(Vector2 a)
{
    return a.x * a.x + a.y * a.y;
}
constexpr float Distance(Vector2 a, Vector2 b)
{
    const float distanceX = a.x - b.x;
    const float distanceY = a.y - b.y;
    return std::sqrt(distanceX * distanceX + distanceY * distanceY);
}
constexpr float SquaredDistance(Vector2 a, Vector2 b)
{
    const float distanceX = a.x - b.x;
    const float distanceY = a.y - b.y;
    return distanceX * distanceX + distanceY * distanceY;
}
constexpr Vector2 Perp(Vector2 a)
{
    return Vector2(-a.y, a.x);
}

constexpr Vector2 Normalize(Vector2 a)
{
    const float length = Length(a);
    if(length == 0.0f)
    {
        return Vector2(0.0f, 0.0f);
    }
    return Vector2(a.x / length, a.y / length);
}

constexpr Vector2 Abs(Vector2 a)
{
    return Vector2(std::abs(a.x), std::abs(a.y));
}

constexpr Vector2 Floor(Vector2 a)
{
    return Vector2(std::floor(a.x), std::floor(a.y));
}

constexpr Vector2 Ceil(Vector2 a)
{
    return Vector2(std::ceil(a.x), std::ceil(a.y));
}

constexpr float Dot(Vector2 a, Vector2 b)
{
    return a.x * b.x + a.y * b.y;
}

constexpr float Cross(Vector2 a, Vector2 b)
{
    return a.x * b.y - a.y * b.x;
}
constexpr Vector2 Lerp(Vector2 a, Vector2 b, float time)
{
    time = std::clamp(time, 0.0f, 1.0f);
    return Vector2{a * (1.0f - time) + b * time};
}
constexpr float RadiansToDeegres(float angleRadians)
{
    return angleRadians * (180.0f / PI);
}
constexpr float DeegresToRadians(float angleDeegres)
{
    return angleDeegres * (PI / 180.0f);
}
constexpr bool NearlyEquel(float a, float b, float absError)
{
    return std::abs(a - b) <= absError;
}
constexpr bool NearlyEquel(Vector2 a, Vector2 b, float absError)
{
    return SquaredDistance(a, b) < absError * absError;
}