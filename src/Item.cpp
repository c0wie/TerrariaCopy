#include "Item.hpp"

Item::Item(short type)
{
    SetItemProperties(type);
}

void Item::SetItemProperties(short type)
{
    auto properties = itemTable[type];
    this->type = type;
    maxStackSize = properties.stackSize;
    range = properties.range;
    damage = properties.damage;
}

bool Item::isWeapon() const
{
    return type == SWORD || type == BOW; 
}

bool Item::isBlock() const
{
    return (!isTool() && !isWeapon());
}

bool Item::isTool() const
{
    return type == PICKAXE || type == AXE ;
}

std::array<Item::itemProperties, Item::ItemType::ITEM_COUNT> Item::itemTable =
{   
    itemProperties{0.0f, 0.0f, 0},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 0.0f, 64},
    itemProperties{5.0f, 70.0f, 1},
    itemProperties{5.0f, 100.0f, 1},
    itemProperties{5.0f, 50.0f, 1},
    itemProperties{20.0f, 20.0f, 1}
};