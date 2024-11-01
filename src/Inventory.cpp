#include "Inventory.hpp"

void Inventory::Draw(Vector2 mousePos, sf::RenderWindow &window, char gameState) const
{
    constexpr float ITEM_BUFFER = 37.0f;
    constexpr float SCREEN_BUFFER = 25.0f;
    const sf::Vector2f screenCenter = window.getView().getCenter();
    const unsigned int maxRows = gameState == GS_MAP? 1 : INVENTORY_HEIGHT;

    for(int col = 0; col < INVENTORY_WIDTH; col++)
    {
        for(int row = 0; row < maxRows; row++)
        {
            const Vector2 itemPos = Vector2(screenCenter.x - SCREEN_WIDTH / 2.0f + col * ITEM_BUFFER + SCREEN_BUFFER,
                                screenCenter.y - SCREEN_HEIGHT / 2.0f + (row * ITEM_BUFFER) + SCREEN_BUFFER);

            inventory[row * INVENTORY_WIDTH + col].Draw(itemPos, (row * INVENTORY_WIDTH + col) == currentItemSlot, false, window);
        }
    }
    if(inventory[inventory.size() - 1].type != Item::Type::NONE)
    {
        inventory[inventory.size() - 1].Draw(mousePos + Vector2{20.0f, 10.0f}, false, true, window);
    }
}

void Inventory::Update()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    {
        currentItemSlot = 0;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    {
        currentItemSlot = 1;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
    {
        currentItemSlot = 2;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
    {
        currentItemSlot = 3;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
    {
        currentItemSlot = 4;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
    {
        currentItemSlot = 5;
    }
}

void Inventory::FindSlotForItem(short itemType)
{
    if(itemType == Item::Type::NONE)
    {
        return;
    }

    int firstEmptyIndex = -1;
    for(int i = 0; i < inventory.size(); i++)
    {
        if(inventory[i].type == itemType && inventory[i].currentStackSize != inventory[i].maxStackSize)
        {
            inventory[i].currentStackSize++;
            return;
        }
        if(firstEmptyIndex == -1 && inventory[i].type == Item::Type::NONE)
        {
            firstEmptyIndex = i;
        }
    }
    // no more free space in inventory
    if(firstEmptyIndex == -1)
    {
        return;
    }
    inventory[firstEmptyIndex].SetItemProperties(itemType);
    inventory[firstEmptyIndex].currentStackSize++;
}

void Inventory::PlaceItem(Vector2 slotCoords)
{
    Item itemInSlot = SafeGetItem(slotCoords);
    Item itemInHand = inventory[inventory.size() - 1];
    if(itemInSlot.type == Item::Type::NONE)
    {
        inventory[slotCoords.y * INVENTORY_WIDTH + slotCoords.x] = itemInHand;
        inventory[inventory.size() - 1] = Item::Type::NONE;
        currentItemSlot = 0;
    }
    else 
    {
        inventory[inventory.size() - 1] = itemInSlot;
        inventory[slotCoords.y * INVENTORY_WIDTH + slotCoords.x] = itemInHand;
        currentItemSlot = inventory.size() - 1;
    }
}

void Inventory::PickItem(Vector2 slotCoords)
{
    Item item = UnsafeGetItem(slotCoords);
    if(item.type == Item::Type::NONE)
    {
        return;
    }
    inventory[slotCoords.y * INVENTORY_WIDTH + slotCoords.x] = Item::Type::NONE;
    inventory[inventory.size() - 1] = item;
    currentItemSlot = inventory.size() - 1;
}

bool Inventory::IsItemHeld() const
{
    return inventory[inventory.size() - 1].type != Item::Type::NONE;
}

Item &Inventory::SafeGetItem(Vector2 slotCoords)
{
    static Item stub{};
    if(IsValidCoords(slotCoords))
    {
        return inventory[slotCoords.y * INVENTORY_WIDTH + slotCoords.x];
    }
    std::cout << "Unsafe interaction\n";
    return stub;
}

Item &Inventory::UnsafeGetItem(Vector2 slotCoords)
{
    return inventory[slotCoords.y * INVENTORY_WIDTH + slotCoords.x];
}

Item &Inventory::GetCurrentItem()
{
    return inventory[currentItemSlot];
}

bool Inventory::IsValidCoords(Vector2 slotCoords) const
{
    if(slotCoords.x < 0 || slotCoords.x > INVENTORY_WIDTH)
    {
        return false;
    }
    if(slotCoords.y < 0 || slotCoords.y > INVENTORY_HEIGHT)
    {
        return false;
    }
    return true;
}

Item &Inventory::operator[](size_t index)
{
    return inventory[index];
}

const Item &Inventory::operator[](size_t index) const
{
    return inventory[index];
}

size_t Inventory::Size() const
{
    return inventory.size();
}