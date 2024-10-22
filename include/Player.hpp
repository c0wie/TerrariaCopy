#pragma once
#include "Global.hpp"
#include "Item.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player() = default;
    void Move(Vector2 offset) { position += offset; }
    void Draw(sf::RenderWindow &window) const;
    void Update(float deltaTime);
    void Save();
    void Load();
    Item &GetItemInHand();
    void FindPlaceForItemInInventory(short type);
    void PlaceBlock();
    Vector2 GetCoords() const;
public:
    Vector2 position{200.0f, 150.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{1.76f * TILE_SIZE, 2.7f * TILE_SIZE};
    float jumpHeight{TILE_SIZE * 2.0f};
    float speed{TILE_SIZE * 7.8f};
    float spaceHoldTime{0.1f};
    bool canJump{false};
    bool canPlaceBlock{true};
    float canPlaceBlockTimer{PLAYER_PLACE_BLOCK_DELAY};
    float strength{100.0f};
private:
    std::array<Item, 6> itemSlots
    {
        Item{Item::ItemType::NONE},
        Item{Item::ItemType::NONE},
        Item{Item::ItemType::NONE},
        Item{Item::ItemType::NONE},
        Item{Item::ItemType::PICKAXE},
        Item{Item::ItemType::SWORD}
    };
    int currentItemSlot{0};
};