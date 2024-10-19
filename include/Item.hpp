#pragma once
#include <array>

struct Item
{
public:
    enum ItemType
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
        BOW,
        ITEM_COUNT
    };
    struct itemProperties
    {
        itemProperties(float range, float dmg, unsigned int stackSize) :
            range(range), damage(dmg), stackSize(stackSize) {}
        float range{0.0f};
        float damage{0.0f};
        unsigned int stackSize{64};
    } ;
    
public:
    Item(short type);
public:
    void SetItemProperties(short type);
    bool isWeapon() const;
    bool isBlock() const;
    bool isTool() const;
public:
    static std::array<itemProperties, ITEM_COUNT> itemTable;
    int currentStackSize{0};
    unsigned int maxStackSize{64};
    float range{0.0f};
    float damage{0.0f};
    short type{NONE};
};