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
        PICKAXE,
        SWORD,
        AXE,
        ITEM_COUNT
    };
public:
    Item() = default;
    Item(short Type);
public:
    void Draw(Vector2 position, bool isActive, bool isInHand, sf::RenderWindow &window) const;
    void SetProperties(short Type);
    bool IsWeapon() const;
    bool IsBlock() const;
    bool IsTool() const;
    bool IsNone() const;
    static void InitBackground();
    static void LoadTextures();
public:
    int currentStackSize{0};
    int maxStackSize{64};
    float damage{0.0f};
    short type{NONE};
    std::shared_ptr<sf::Sprite> sprite{std::make_shared<sf::Sprite>()};
    static sf::RectangleShape background;
};