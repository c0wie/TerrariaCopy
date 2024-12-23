#pragma once
#include <iostream>
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>

//Basic struct for manipulating 2-dimensional vectors
struct Vector2
{
public:
    constexpr Vector2() = default;
    constexpr Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const sf::Vector2f& vec)
        : x(vec.x), y(vec.y) {}
    Vector2(const sf::Vector2i& vec)
        : x(vec.x), y(vec.y) {} 
    Vector2(const sf::Vector2u& vec)
        : x(vec.x), y(vec.y) {} 
    operator sf::Vector2f() const 
    {
        return sf::Vector2f(x, y);
    }
    operator sf::Vector2i() const 
    {
        return sf::Vector2i(static_cast<float>(x), static_cast<float>(y)); 
    }
public:
    constexpr bool operator==(Vector2 other) const
    {
        return x == other.x && y == other.y;
    }
    constexpr bool operator!=(Vector2 other) const
    {
        return x != other.x && y != other.y;
    }
    constexpr bool operator <=(Vector2 other) const
    {
        return (x <= other.x && y <= other.y);
    }
    constexpr Vector2 operator+(Vector2 other) const
    {
        return Vector2{ x + other.x, y + other.y};
    }
    constexpr Vector2 operator-(Vector2 other) const
    {
        return Vector2{x - other.x, y - other.y};
    }      
    constexpr Vector2& operator+=(Vector2 other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    constexpr Vector2& operator+=(float number)
    {
        x += number;
        y += number;
        return *this;
    }
    constexpr Vector2& operator-=(Vector2 other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    constexpr Vector2& operator-=(float number)
    {
        x -= number;
        y -= number;
        return *this;
    }
    constexpr Vector2 operator*(Vector2 other) const
    {
        return Vector2{x * other.x, y * other.y};
    }
    constexpr Vector2 operator*(float multiplier) const
    {
        return Vector2{ x * multiplier, y * multiplier};
    }
    friend constexpr Vector2 operator*(float multiplier, const Vector2& vec)
    {
        return vec * multiplier;
    }
    constexpr Vector2& operator*=(float multiplier)
    {
        x *= multiplier;
        y *= multiplier;
        return *this;
    }
    constexpr Vector2 operator/(float divider) const
    {
         return Vector2{ x / divider, y / divider};
    }
    friend constexpr Vector2 operator/(float divider, const Vector2& vec)
    {
        return vec / divider;
    }
    constexpr Vector2 operator/(Vector2 divider) const
    {
         return Vector2{ x / divider.x, y / divider.y};
    }
    std::string GetString() const
    {
        std::string name = std::to_string(x) + ";" + std::to_string(y);
        return name;
    }
public:
    float x{0.0f};
    float y{0.0f};
};

// format of string has to be vec.x;vec.y
inline Vector2 ExtaractVector2FromString(const std::string &line)
{
    int semicolonPos = line.find(';');
    if(semicolonPos == std::string::npos)
    {
        std::cerr << "Invalid format: no semicolon found.\n";
        std::exit(1);
    }
    std::string xStr = line.substr(0, semicolonPos);
    std::string yStr = line.substr(semicolonPos + 1);
    return Vector2{(float)std::stoi(xStr), (float)std::stoi(yStr)};
}