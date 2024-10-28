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

void Inventory::FindPlaceForItemInInventory(short type)
{
    if(type == Item::Type::NONE)
    {
        return;
    }

    for(int i = 0; i < inventory.size(); i++)
    {
        if(inventory[i].type == (int)type && inventory[i].currentStackSize != inventory[i].maxStackSize)
        {
            inventory[i].currentStackSize++;
            return;
        }
    }
    for(int i = 0; i < inventory.size(); i++)
    {
        if(inventory[i].type == Item::Type::NONE)
        {
            inventory[i].SetItemProperties(type);
            inventory[i].currentStackSize++;
            return;
        }
    }
}

void Inventory::PutItemInTheSlot(Vector2 coords)
{
    Item itemInSlot = SafeGetItem(coords);
    Item itemInHand = inventory[inventory.size() - 1];
    if(itemInSlot.type == Item::Type::NONE)
    {
        inventory[coords.y * INVENTORY_WIDTH + coords.x] = itemInHand;
        inventory[inventory.size() - 1] = Item::Type::NONE;
        currentItemSlot = 0;
    }
    else 
    {
        inventory[inventory.size() - 1] = itemInSlot;
        inventory[coords.y * INVENTORY_WIDTH + coords.x] = itemInHand;
        currentItemSlot = inventory.size() - 1;
    }
}

void Inventory::PickItemFromInventory(Vector2 coords)
{
    Item item = UnsafeGetItem(coords);
    if(item.type == Item::Type::NONE)
    {
        return;
    }
    inventory[coords.y * INVENTORY_WIDTH + coords.x] = Item::Type::NONE;
    inventory[inventory.size() - 1] = item;
    currentItemSlot = inventory.size() - 1;
}

Item &Inventory::GetItemInHand()
{
    return inventory[currentItemSlot];
}

bool Inventory::IsItemHeld() const
{
    return inventory[inventory.size() - 1].type != Item::Type::NONE;
}

Item &Inventory::SafeGetItem(Vector2 coords)
{
    static Item stub{};
    if(IsValidCoords(coords))
    {
        return inventory[coords.y * INVENTORY_WIDTH + coords.x];
    }
    std::cout << "Unsafe interaction\n";
    return stub;
}

Item &Inventory::UnsafeGetItem(Vector2 coords)
{
    return inventory[coords.y * INVENTORY_WIDTH + coords.x];
}

Item &Inventory::GetCurrentItem()
{
    return inventory[currentItemSlot];
}

bool Inventory::IsValidCoords(Vector2 coords) const
{
    if(coords.x < 0 || coords.x > INVENTORY_WIDTH)
    {
        return false;
    }
    if(coords.y < 0 || coords.y > INVENTORY_HEIGHT)
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