#include "Player.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "imgui.h"

namespace fs = std::filesystem;

Player::Player(bool generate)
{
    if(!generate)
    {
        LoadPlayer();
    }
}

Player::~Player()
{
    SavePlayer();
}

void Player::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape player(size);
    player.setOrigin(size / 2.0f);
    player.setPosition(position);
    player.setFillColor(sf::Color::Red);
    window.draw(player);

    ImGui::Begin("Inventory");
    ImGui::Text("Current item: %i", itemSlots[currentItemSlot].ID);
    ImGui::Text("1. %i, %i", itemSlots[0].ID, itemSlots[0].currentStackSize); ImGui::SameLine; 
    ImGui::Text("2. %i, %i", itemSlots[1].ID, itemSlots[1].currentStackSize); ImGui::SameLine; 
    ImGui::Text("3. %i, %i", itemSlots[2].ID, itemSlots[2].currentStackSize); ImGui::SameLine; 
    ImGui::Text("4. %i, %i", itemSlots[3].ID, itemSlots[3].currentStackSize); ImGui::SameLine; 
    ImGui::Text("5. %i, %i", itemSlots[4].ID, itemSlots[4].currentStackSize); ImGui::SameLine; 
    ImGui::Text("6. %i, %i", itemSlots[5].ID, itemSlots[5].currentStackSize); ImGui::SameLine; 
    ImGui::End();
}

void Player::Update(float deltaTime)
{
#pragma region handle movemant
    velocity.x = 0.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        isSprinting = true;
    }
    else
    {
        isSprinting = false;
    }
    const float multiplier = isSprinting? 1.5f : 1.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        velocity.x -= speed * multiplier;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += speed * multiplier;
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

void Player::SavePlayer()
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
        playerData << itemSlots[i].ID << ',' << itemSlots[i].currentStackSize << '\n';
    }
    playerData.close();
}

void Player::LoadPlayer()
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
        itemSlots[i].SetItemProperties((ItemType)std::stoi(itemType));
        itemSlots[i].currentStackSize = std::stoi(itemCount);
        i++;
    }
}

int Player::GetItemInHand(int &purpose) const
{
    purpose = itemSlots[currentItemSlot].purpose;
    return itemSlots[currentItemSlot].ID;
}

void Player::FindPlaceForItemInInventory(ItemType type)
{
    if(type == ItemType::NONE)
    {
        return;
    }
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
            itemSlots[i].currentStackSize++;
            return;
        }
    }
}

void Player::PlaceBlock()
{
    itemSlots[currentItemSlot].currentStackSize--;
    if(itemSlots[currentItemSlot].currentStackSize <= 0)
    {
        itemSlots[currentItemSlot] = itemTable[(int)ItemType::NONE];
    }
}