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
    void FindSlotForItem(short type);
    void PlaceItem(Vector2 coords);
    void PickItem(Vector2 coords);
    bool IsItemHeld() const;
    Item &SafeGetItem(Vector2 slotCoords);
    Item &UnsafeGetItem(Vector2 slotCoords);
    Item &GetCurrentItem();
    Item& operator[](size_t index); 
    const Item& operator[](size_t index) const;
    size_t Size() const;
private:
    bool IsValidCoords(Vector2 slotCoords) const;
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