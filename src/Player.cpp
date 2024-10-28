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
    for(int i = 0; i < inventory.Size(); i++)
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

void Player::PlaceBlock()
{
    Item &currentItem = inventory.GetCurrentItem();
    if(currentItem.currentStackSize >= 0)
    {
        currentItem.currentStackSize--;
    }
    if(currentItem.currentStackSize < 0)
    {
        currentItem.SetItemProperties(Item::Type::NONE);
    }
}

Vector2 Player::GetCoords() const
{
    return Floor(position / TILE_SIZE);
}