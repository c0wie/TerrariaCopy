#pragma once
#include "Global.hpp"
#include "Item.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

constexpr char INVENTORY_PLAYER_GUI = 1;
constexpr char GAME_PLAYER_GUI = 0;
constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.21;
constexpr float DAMAGE_TRESHOLD_SPEED = 500.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
class Player
{
public:
    Player() = default;
    void Move(Vector2 offset) { position += offset; }
    void Draw(sf::RenderWindow &window, char gameState) const;
    void Update(float deltaTime, char gameState);
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
    int health {100};
private:
    std::array<Item, 6> itemSlots
    {
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::NONE},
        Item{Item::Type::PICKAXE},
        Item{Item::Type::SWORD}
    };
    int currentItemSlot{0};
    unsigned char playerGUIState {GAME_PLAYER_GUI};
};