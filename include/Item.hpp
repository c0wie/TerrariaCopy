#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "Vector2.hpp"
#include <memory>

struct Item
{
public:
    enum Type
    {
        NONE = 0,
        STONE,
        GRASS,
        LOG,
        IRON,
        COPPER,
        GOLD,
        SILVER,
        PICKAXE,
        SWORD,
        AXE,
        ITEM_COUNT
    };
public:
    Item() = default;
    Item(short type);
public:
    void Draw(Vector2 position, bool isActive, bool isInHand, sf::RenderWindow &window) const;
    void SetItemProperties(short type);
    bool IsWeapon() const;
    bool IsBlock() const;
    bool IsTool() const;
    bool IsNone() const;
public:
    void LoadTexture();
    int currentStackSize{-1};
    // when it you want it to be 64 it is 0 - 63
    int maxStackSize{63};
    float damage{0.0f};
    short type{NONE};
    std::shared_ptr<sf::Texture> txt{std::make_shared<sf::Texture>()};
};