#include "Global.hpp"

void Item::SetItemProperties(ItemType type)
{
    Item item = itemTable[(int)type];
    ID = item.ID;
    maxStackSize = item.maxStackSize;
    currentStackSize = item.currentStackSize;
    purpose = item.purpose;
}