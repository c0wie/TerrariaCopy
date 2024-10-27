#include "Player.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "imgui.h"

namespace fs = std::filesystem;

void Player::Draw(Vector2 mousePos, sf::RenderWindow &window, char gameState) const
{
    constexpr float ITEM_BUFFER = 37.0f;
    constexpr float SCREEN_BUFFER = 25.0f;
    sf::RectangleShape player(size);
    player.setOrigin(size / 2.0f);
    player.setPosition(position);
    player.setFillColor(sf::Color::Red);
    window.draw(player);

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

void Player::Update(float deltaTime, char gameState)
{
#pragma region handle fall damage
    static float fallDamage = 0.0f;
    if(canJump && fallDamage != 0.0f)
    {
        std::cout << "You've taken damage:" << health - int(health - fallDamage) << '\n';
        health -= fallDamage;
        fallDamage = 0.0f;
    }
#pragma endregion

#pragma region handle movement
    velocity.x = 0.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        velocity.x -= speed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += speed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canJump)
    {
        if(spaceHoldTime < MAX_SPACE_HOLD_TIME)
        {
            spaceHoldTime += deltaTime;
        }
        else
        {
            canJump = false;
        }
        velocity.y = -sqrtf(2.0f * GRAVITY * jumpHeight * spaceHoldTime / MAX_SPACE_HOLD_TIME);
    }
    else 
    {
        spaceHoldTime = 0.0f;
        canJump = false;
    }
    velocity.y += GRAVITY * deltaTime;
    if(velocity.y >= DAMAGE_TRESHOLD_SPEED)
    {
        fallDamage += (velocity.y - DAMAGE_TRESHOLD_SPEED) * 0.007 > 1.0f? (velocity.y - DAMAGE_TRESHOLD_SPEED) * 0.007 : 0.0f;
    }   

#pragma endregion  

#pragma region handle block placemant
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && canPlaceBlock)
    {
        canPlaceBlockTimer = 0.0f;
        canPlaceBlock = false;
    }
    else
    {
        canPlaceBlockTimer += deltaTime;
    }
    if(canPlaceBlockTimer >= PLAYER_PLACE_BLOCK_DELAY)
    {
        canPlaceBlock = true;
    }
#pragma endregion

#pragma region handle inventory 
    
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
    if(gameState == GS_INVENTORY)
    {

    }
#pragma endregion
}

void Player::Save()
{
    fs::path filePath = "SAVE_PLAYER.csv";
    if(!fs::exists(filePath))
    {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return;
    }
    std::ofstream playerData(filePath);
    if(!playerData.is_open())
    {   
        std::cerr << "Can't open the file: " << filePath << std::endl;
        return;
    }
    playerData << position.x << ';' << position.y << '\n';
    for(int i = 0; i < inventory.size(); i++)
    {
        playerData << inventory[i].type << ',' << inventory[i].currentStackSize << '\n';
    }
    playerData.close();
}

void Player::Load()
{
    fs::path filePath = "SAVE_PLAYER.csv";
    if(!fs::exists(filePath))
    {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return;
    }
    std::ifstream mapData(filePath);
    if(!mapData.is_open())
    {   
        std::cerr << "Can't open the file: " << filePath << std::endl;
        return;
    }
    std::string line;
    std::getline(mapData, line);
    position = ExtaractVector2FromString(line);
    int i = 0;
    while(std::getline(mapData, line))
    {
        if(line.empty())
        {
            continue;
        }
        int commaPosition = line.find(',');
        if(commaPosition == std::string::npos)
        {
            std::cout << line << '\n';
            std::cerr << "Invalid format: no commma found.\n";
            std::exit(1);
        }
        std::string itemType = line.substr(0, commaPosition);
        std::string itemCount = line.substr(commaPosition + 1);
        inventory[i].SetItemProperties(std::stoi(itemType));
        inventory[i].currentStackSize = std::stoi(itemCount);
        i++;
    }
}

Item &Player::GetItemInHand()
{
    return inventory[currentItemSlot];
}

void Player::FindPlaceForItemInInventory(short type)
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

void Player::PlaceBlock()
{
    if(inventory[currentItemSlot].currentStackSize >= 0)
    {
        inventory[currentItemSlot].currentStackSize--;
    }
    if(inventory[currentItemSlot].currentStackSize < 0)
    {
        inventory[currentItemSlot].SetItemProperties(Item::Type::NONE);
    }
}

Vector2 Player::GetCoords() const
{
    return Floor(position / TILE_SIZE);
}

bool Player::IsItemHeld() 
{
    return inventory[inventory.size() - 1].type != Item::Type::NONE;
}

Item &Player::SafeGetItem(Vector2 coords)
{
    static Item stub{};
    if(IsValidCoords(coords))
    {
        return inventory[coords.y * INVENTORY_WIDTH + coords.x];
    }
    std::cout << "Unsafe interaction\n";
    return stub;
}

Item &Player::UnsafeGetItem(Vector2 coords)
{
    return inventory[coords.y * INVENTORY_WIDTH + coords.x];
}

bool Player::IsValidCoords(Vector2 coords) const
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

void Player::PutItemInTheSlot(Vector2 coords)
{
    Item itemInSlot = SafeGetItem(coords);
    Item itemInHand = inventory[inventory.size() - 1];
    itemInHand.isHeld = false;
    if(itemInSlot.type == Item::Type::NONE)
    {
        inventory[coords.y * INVENTORY_WIDTH + coords.x] = itemInHand;
        inventory[inventory.size() - 1] = Item::Type::NONE;
        currentItemSlot = 0;
    }
    else 
    {
        itemInSlot.isHeld = true;
        inventory[inventory.size() - 1] = itemInSlot;
        inventory[coords.y * INVENTORY_WIDTH + coords.x] = itemInHand;
        currentItemSlot = inventory.size() - 1;
    }
}

void Player::PickItemFromInventory(Vector2 coords)
{
    Item item = UnsafeGetItem(coords);
    if(item.type == Item::Type::NONE)
    {
        return;
    }
    inventory[coords.y * INVENTORY_WIDTH + coords.x] = Item::Type::NONE;
    item.isHeld = true;
    inventory[inventory.size() - 1] = item;
    currentItemSlot = inventory.size() - 1;
}