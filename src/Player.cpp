#include "Player.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "imgui.h"

namespace fs = std::filesystem;

void Player::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape player(size);
    player.setOrigin(size / 2.0f);
    player.setPosition(position);
    player.setFillColor(sf::Color::Red);
    window.draw(player);

    const sf::Vector2f screenCenter = window.getView().getCenter();
    for(int i = 0; i < itemSlots.size(); i++)
    {
        itemSlots[i].Draw(Vector2(screenCenter.x - SCREEN_WIDTH / 2.0f + i * 50.0f + 25.0f,
                                screenCenter.y - SCREEN_HEIGHT / 2.0f + 25.0f), i == currentItemSlot ,window);
    }
}

void Player::Update(float deltaTime)
{
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
    for(int i = 0; i < itemSlots.size(); i++)
    {
        playerData << itemSlots[i].type << ',' << itemSlots[i].currentStackSize << '\n';
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
        itemSlots[i].SetItemProperties(std::stoi(itemType));
        itemSlots[i].currentStackSize = std::stoi(itemCount);
        i++;
    }
}

Item &Player::GetItemInHand()
{
    return itemSlots[currentItemSlot];
}

void Player::FindPlaceForItemInInventory(short type)
{
    if(type == Item::ItemType::NONE)
    {
        return;
    }

    for(int i = 0; i < itemSlots.size(); i++)
    {
        if(itemSlots[i].type == (int)type && itemSlots[i].currentStackSize != itemSlots[i].maxStackSize)
        {
            itemSlots[i].currentStackSize++;
            return;
        }
    }
    for(int i = 0; i < itemSlots.size(); i++)
    {
        if(itemSlots[i].type == Item::ItemType::NONE)
        {
            itemSlots[i].SetItemProperties(type);
            itemSlots[i].currentStackSize++;
            return;
        }
    }
}

void Player::PlaceBlock()
{
    if(itemSlots[currentItemSlot].currentStackSize > 0)
    {
        itemSlots[currentItemSlot].currentStackSize--;
    }
    if(itemSlots[currentItemSlot].currentStackSize <= 0)
    {
        itemSlots[currentItemSlot].SetItemProperties(Item::ItemType::NONE);
    }
}

Vector2 Player::GetCoords() const
{
    return Floor(position / TILE_SIZE);
}