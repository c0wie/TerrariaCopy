#pragma once

#include "Global.hpp"
#include "Item.hpp"
#include <SFML/Graphics.hpp>
#include <array>

constexpr int INVENTORY_HEIGHT = 4;
constexpr int INVENTORY_WIDTH = 6;

class Inventory
{
public:
    void Draw(Vector2 mousePos, sf::RenderWindow &window) const;
    void Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event);
    void FindSlotForItem(short type);
    void PutItemAside(Vector2 coords);
    void PickItem(Vector2 coords);
    void PlaceBlock();
    bool IsItemHeld() const;
    Item &SafeGetItem(Vector2 slotCoords);
    Item &UnsafeGetItem(Vector2 slotCoords);
    Item &GetCurrentItem();
    Item& operator[](size_t index); 
    const Item& operator[](size_t index) const;
    size_t Size() const;
private:
    bool IsValidCoords(Vector2 slotCoords) const;
public:
    bool isMouseInInventory{false};
private:
    std::array<Item, (INVENTORY_WIDTH * INVENTORY_HEIGHT) + 1> inventory
    {
        Item{Item::Type::SWORD},
        Item{Item::Type::PICKAXE},
        Item{Item::Type::TORCH, 5},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::IRON},
        Item{Item::Type::COPPER},
        Item{Item::Type::SILVER},
        Item{Item::Type::GOLD},
        Item{Item::Type::NONE}
    };
    int currentItemSlot{0};
    bool isInventoryOpened{false};
};