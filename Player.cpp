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
    ImGui::Text("Current item: %i", itemSlots[currentItemSlot].first);
    ImGui::Text("1. %i", itemSlots[0].first); ImGui::SameLine; ImGui::Spacing();
    ImGui::Text("2. %i", itemSlots[1].first); ImGui::SameLine; ImGui::Spacing();
    ImGui::Text("3. %i", itemSlots[2].first); ImGui::SameLine; ImGui::Spacing();
    ImGui::Text("4. %i", itemSlots[3].first); ImGui::SameLine; ImGui::Spacing();
    ImGui::Text("5. %i", itemSlots[4].first); ImGui::SameLine; ImGui::Spacing();
    ImGui::Text("6. %i", itemSlots[5].first); ImGui::SameLine; ImGui::Spacing();
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

    int semicolonPos = line.find(';');
    if(semicolonPos == std::string::npos)
    {
        std::cerr << "Invalid format: no semicolon found.\n";
        std::exit(1);
    }

    std::string xStr = line.substr(0, semicolonPos);
    std::string yStr = line.substr(semicolonPos + 1);
    position.x = std::stoi(xStr);  // Convert x position to an integer
    position.y = std::stoi(yStr);  // Convert y position to an integer
}