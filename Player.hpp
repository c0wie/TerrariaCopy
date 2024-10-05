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
    int GetItemInHand(bool &isBlock) const
    {
        isBlock = itemSlots[currentItemSlot].second;
        return itemSlots[currentItemSlot].first;
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
    std::array<std::pair<int, bool>, 6> itemSlots
    {
        std::pair{(int)TileType::STONE, true},
        std::pair{(int)TileType::GRASS, true},
        std::pair{(int)TileType::BORDER, true},
        std::pair{(int)TileType::LOG, true},
        std::pair{(int)TileType::AIR, true},
        std::pair{(int)TileType::LOG, false}
    };
    int currentItemSlot{0};
};