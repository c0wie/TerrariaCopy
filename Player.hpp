#pragma once
#include "Global.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player(bool generate);
    ~Player();
    void Move(Vector2 offset) { position += offset; }
    void Draw(sf::RenderWindow &window) const;
    void Update(float deltaTime);
    void SavePlayer();
    void LoadPlayer();
    int GetItemInHand(int &purpose) const
    {
        purpose = itemSlots[currentItemSlot].purpose;
        return itemSlots[currentItemSlot].ID;
    }
    void FindPlaceForItemInInventory(ItemType type)
    {
        for(int i = 0; i < itemSlots.size(); i++)
        {
            if(itemSlots[i].ID == (int)type && itemSlots[i].currentStackSize != itemSlots[i].maxStackSize)
            {
                itemSlots[i].currentStackSize++;
                return;
            }
        }
        for(int i = 0; i < itemSlots.size(); i++)
        {
            if(itemSlots[i].ID == (int)ItemType::NONE)
            {
                itemSlots[i] = itemTable[(int)type];
                return;
            }
        }
    };
    void PlaceBlock()
    {
        if(itemSlots[currentItemSlot].currentStackSize <= 0)
        {
            itemSlots[currentItemSlot] = itemTable[(int)ItemType::NONE];
        }
        else
        {
            itemSlots[currentItemSlot].currentStackSize--;
        }
    }
public:
    Vector2 position{100.0f, 50.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{TILE_SIZE, 1.5f * TILE_SIZE};
    float jumpHeight{100.0f};
    float speed{200.0f};
    float spaceHoldTime{0.1f};
    bool canJump{false};
    bool isSprinting{false};
    bool canPlaceBlock{true};
    float canPlaceBlockTimer{PLAYER_PLACE_BLOCK_DELAY};
    float strength{100.0f};
private:
    std::array<Item, 6> itemSlots
    {
        itemTable[(int)ItemType::NONE],
        itemTable[(int)ItemType::NONE],
        itemTable[(int)ItemType::NONE],
        itemTable[(int)ItemType::NONE],
        itemTable[(int)ItemType::PICKAXE],
        itemTable[(int)ItemType::SWORD]
    };
    int currentItemSlot{0};
};