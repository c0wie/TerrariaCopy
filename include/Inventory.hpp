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
    void Draw(Vector2 mousePos, sf::RenderWindow &window, char gameState) const;
    void Update();
    void FindPlaceForItemInInventory(short type);
    void PutItemInTheSlot(Vector2 coords);
    void PickItemFromInventory(Vector2 coords);
    Item &GetItemInHand();
    bool IsItemHeld() const;
    Item &SafeGetItem(Vector2 coords);
    Item &UnsafeGetItem(Vector2 coords);
    Item &GetCurrentItem();
    Item& operator[](size_t index); 
    const Item& operator[](size_t index) const;
    size_t Size() const;
private:
    bool IsValidCoords(Vector2 coords) const;
private:
    std::array<Item, (INVENTORY_WIDTH * INVENTORY_HEIGHT) + 1> inventory
    {
        Item{Item::Type::SWORD},
        Item{Item::Type::PICKAXE},
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
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE}
    };
    int currentItemSlot{0};
};