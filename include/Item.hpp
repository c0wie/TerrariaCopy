#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "Vector2.h"
#include <memory>

struct ItemProperties
{
    ItemProperties() {}
    ItemProperties(int textureID_, int maxStackSize_, float damage_) :
        txtID(textureID_), maxStackSize(maxStackSize_), damage(damage_) {}
    sf::Texture txt;
    float damage{0.0f};
    int maxStackSize{64};
    int txtID{0};
};

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
        TORCH,
        PICKAXE,
        SWORD,
        AXE,
        ITEM_COUNT
    };
public:
    Item() = default;
    Item(short Type, int count = 1);
public:
    void Draw(bool isActive, bool isInHand, sf::RenderWindow &window) const;
    void SetProperties(short Type, int count = 1);
    bool IsWeapon() const;
    bool IsBlock() const;
    bool IsTool() const;
    bool IsNone() const;
    bool CanBeHang() const;
    static void InitBackground();
    static void LoadTextures();

    void SetPosition(Vector2 newPosition);
public:
    sf::Sprite sprite;
    float damage{0.0f};
    int currentStackSize{0};
    int maxStackSize{64};
    short type{NONE};
    static sf::RectangleShape background;
};