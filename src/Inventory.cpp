#include "Inventory.hpp"
#include "Collision.hpp"

void Inventory::Draw(Vector2 mousePos, sf::RenderWindow &window) const
{
    const unsigned int maxRows = isInventoryOpened? INVENTORY_HEIGHT : 1;
    for(int col = 0; col < INVENTORY_WIDTH; col++)
    {
        for(int row = 0; row < maxRows; row++)
        {
            inventory[row * INVENTORY_WIDTH + col].Draw((row * INVENTORY_WIDTH + col) == currentItemSlot, false, window);
        }
    }
    if(inventory[inventory.size() - 1].type != Item::Type::NONE)
    {
        inventory[inventory.size() - 1].Draw(false, true, window);
    }
}

void Inventory::Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event)
{
#pragma region calculate inventory boundaries 
    const Vector2 topLeftInventoryCorner = {windowCenter.x - SCREEN_WIDTH / 2.0f + 12.5f, windowCenter.y - SCREEN_HEIGHT / 2.0f + 12.5f};
    const Vector2 botRightInventoryCorner = {windowCenter.x - SCREEN_WIDTH / 2.0f + 225.f, windowCenter.y - SCREEN_HEIGHT / 2.0f + 152.5f};
    isMouseInInventory = isInventoryOpened? PointRectCollision(mousePos, topLeftInventoryCorner, botRightInventoryCorner) : false;
#pragma endregion 

#pragma region open/close inventory
    if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
    {
        isInventoryOpened = !isInventoryOpened;
    }
#pragma endregion

#pragma region grabbing/putting away item 
    if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && isMouseInInventory)
    {
        const Vector2 itemCoords = Vector2{ int((mousePos.x - topLeftInventoryCorner.x) / 37), int((mousePos.y - topLeftInventoryCorner.y) / 37) };
        if(IsItemHeld())
        {
            PutItemAside(itemCoords);
        }
        else
        {
            PickItem(itemCoords);
        }
    }
#pragma endregion

#pragma region slot change
    if(!IsItemHeld())
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
    else
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        {
            currentItemSlot = 0;
            PutItemAside({0, 0});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        {
            currentItemSlot = 1;
            PutItemAside({1, 0});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        {
            currentItemSlot = 2;
            PutItemAside({2, 0});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
        {
            currentItemSlot = 3;
            PutItemAside({3, 0});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
        {
            currentItemSlot = 4;
            PutItemAside({4, 0});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
        {
            currentItemSlot = 5;
            PutItemAside({5, 0});
        }
    }
#pragma endregion

#pragma region set position of items on screen
    constexpr float ITEM_BUFFER = 37.0f;
    constexpr float SCREEN_BUFFER = 25.0f;
    const unsigned int maxRows = isInventoryOpened? INVENTORY_HEIGHT : 1;
    for(int col = 0; col < INVENTORY_WIDTH; col++)
    {
        for(int row = 0; row < maxRows; row++)
        {
            inventory[row * INVENTORY_WIDTH + col].SetPosition(Vector2(windowCenter.x - (SCREEN_WIDTH / 2.0f) + col * ITEM_BUFFER + SCREEN_BUFFER,
                                windowCenter.y - SCREEN_HEIGHT / 2.0f + (row * ITEM_BUFFER) + SCREEN_BUFFER) );
        }
    }
    if(IsItemHeld())
    {
        inventory[inventory.size() - 1].SetPosition(mousePos + Vector2{20.0f, 10.0f});
    }
#pragma endregion
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
        if(inventory[i].type == itemType && inventory[i].currentStackSize < inventory[i].maxStackSize)
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
    inventory[firstEmptyIndex].SetProperties(itemType);
}

void Inventory::PutItemAside(Vector2 slotCoords)
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

void Inventory::PlaceBlock()
{
    Item &currentItem = GetCurrentItem();
    if(currentItem.currentStackSize > 0)
    {
        currentItem.currentStackSize--;
    }
    if(currentItem.currentStackSize <= 0)
    {
        currentItem.SetProperties(Item::Type::NONE, 0);
    }
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