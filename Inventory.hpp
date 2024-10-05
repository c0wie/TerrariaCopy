#pragma once
#include "Global.hpp"
#include <functional>

constexpr enum class ItemType
{
    NONE,
    GRASS,
    STONE,
    LOG,
    BORDER,
    PICKAXE,
    SWORD,
    AXE,
    BOW,
    COUNT
};

/*
    purpose:
    - 0 special character to represent NONE state
    - 1 item to attack meele and range
    - 2 item to dig or destroy tiles
    - 3 item to place as a tile 

*/
struct Item
{
    constexpr Item(ItemType type, unsigned stackSize, int purpose):
        ID((int)type),
        maxStackSize(stackSize),
        purpose(purpose)
    {}
    int ID{0};
    unsigned int maxStackSize{64};
    int purpose{0};
};

constexpr std::array<Item, (size_t)ItemType::COUNT> itemTable = 
{
    Item(ItemType::NONE, 0, 0),
    Item(ItemType::GRASS, 64, 3),
    Item(ItemType::STONE, 64, 3),
    Item(ItemType::LOG, 64, 3),
    Item(ItemType::BORDER, 64, 3),
    Item(ItemType::PICKAXE, 1, 2),
    Item(ItemType::SWORD, 1, 1),
    Item(ItemType::AXE, 1, 2),
    Item(ItemType::BOW, 1, 1)
};
