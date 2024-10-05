#pragma once
#include <array>
#include <SFML/Graphics.hpp>


constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float GRAVITY = 981.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;
constexpr int TILES_COUNT = 100;
constexpr float TILE_SIZE = 50.0f;
constexpr int MAP_WIDTH = 100;
constexpr int MAP_HEIGHT = 30;
constexpr int INVENTORY_COUNT = 40; 
constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.21;
constexpr unsigned int TILES_TYPE_COUNT = 5;
constexpr unsigned int TOOL_COUNT = 2;
constexpr unsigned int WEAPON_COUNT = 2;

enum class ItemType
{
    NONE,
    GRASS,
    STONE,
    LOG,
    BORDER,
    PICKAXE,
    SWORD,
    AXE,
    BOW,
    COUNT
};

/*
    purpose:
    - 0 special character to represent NONE state
    - 1 item to attack meele and range
    - 2 item to dig or destroy tiles
    - 3 item to place as a tile 

*/
struct Item
{
    constexpr Item(ItemType type, unsigned stackSize, int purpose):
        ID((int)type),
        maxStackSize(stackSize),
        purpose(purpose)
    {}
    int ID{0};
    unsigned int maxStackSize{64};
    int currentStackSize{0};
    int purpose{0};
};

constexpr std::array<Item, (size_t)ItemType::COUNT> itemTable = 
{
    Item(ItemType::NONE, 0, 0),
    Item(ItemType::GRASS, 64, 3),
    Item(ItemType::STONE, 64, 3),
    Item(ItemType::LOG, 64, 3),
    Item(ItemType::BORDER, 64, 3),
    Item(ItemType::PICKAXE, 1, 2),
    Item(ItemType::SWORD, 1, 1),
    Item(ItemType::AXE, 1, 2),
    Item(ItemType::BOW, 1, 1)
};

struct tileProperties
{
    tileProperties(sf::Color color, bool isSolid, bool canPlaceBlock, float durability) :
        color(color), 
        isSolid(isSolid), 
        canPlaceBlock(canPlaceBlock), 
        durability(durability)
    {}
    sf::Color color{sf::Color::Black};
    bool isSolid{true};
    bool canPlaceBlock{false};
    float durability{0.0f};
};

const std::array<tileProperties, TILES_TYPE_COUNT> tilePropertiesTable = 
{
    tileProperties(sf::Color::Transparent, false, false, 0.0f),
    tileProperties(sf::Color::Green, true, true, 100),
    tileProperties({111, 118, 130}, true, true, 300),
    tileProperties(sf::Color::Yellow, false, true, 150),
    tileProperties({164, 52, 235}, true, true, 1000000.0f)
};

struct toolProperties
{
    float range{0.0f};
    float damage{1.0f};
};

constexpr std::array<toolProperties, TOOL_COUNT> toolPropertiesTable
{
    toolProperties(5.0f, 200.0f),
    toolProperties(5.0f, 100.0f)
};

struct weaponProperties
{
    float range{0.0f};
    float damage{0.0f};
};

constexpr std::array<weaponProperties, TOOL_COUNT> weaponPropertiesTable = 
{
    weaponProperties(5.0f, 200.0f),
    weaponProperties(20.0f, 50.0f)
};



