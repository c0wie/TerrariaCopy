#include "Player.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "imgui.h"

namespace fs = std::filesystem;

BodyPart loadedBodyParts[Player::BodyParts::BODY_PARTS_COUNT] = 
{
    {0,  {0, -30}, {38, 54}}, // head
	{1,  {0, -30}, {38, 54}}, // eyewhite
	{2,  {0, -30}, {38, 54}}, // eye
	{3,  {0, -30}, {38, 54}}, // torso
	{7,  {-5, -28}, {38, 54}}, // right arm
	{10, {0, -30}, {38, 54}}, // feet arm
	{6,  {0, -30}, {38, 54}}, // clothes
	{11, {0, -30}, {38, 54}}  // pants
};

sf::Texture hairSprites[20];

void PlayerAnimation::Update(float runSpeed, float deltaTime)
{
    if(state == STAY)
    {
        bool grounded_ = grounded;
        *this = PlayerAnimation{};
        grounded = grounded_;
    }
    /*else if(state == RUN)
    {
        timer += deltaTime;

        if(headFrame >= 6)
        {
            headFrame -= 6;
        }

        while (timer >= runSpeed)
        {
            timer -= runSpeed;
            headFrame++;
        }

        headFrame %= 14;

        if( (headFrame >= 1 && headFrame <= 3) || (headFrame >= 8 && headFrame<= 10) )
        {
            isFrameUp = true;
        }
        else
        {
            isFrameUp = false;
        }

        hairFrame = headFrame;

        handFrameX = (headFrame / 2) % 4 + 3;
        handFrameY = 1;

        headFrame += 6;
    }*/
}

void Player::Draw(Vector2 scale, sf::RenderWindow &window) const
{
    constexpr Vector2 hairOffset = {0.0f, -30.0f}; 
    const int headFrame = animation.headFrame;
    const int hairFrame = animation.hairFrame;

    auto drawPart = [&](int i, Vector2 coords, const float color[3])
    {
        auto &part = loadedBodyParts[i];
        const sf::IntRect rect {coords * loadedBodyParts[i].atlasSize, part.atlasSize};
        sf::Sprite sprite(part.txt, rect);
        if(!isMovingRight)
        {
            // flip in x axes
            sprite.setScale(-1.f * scale.x, 1.f * scale.y);
        }
        sprite.setColor(sf::Color{color[0], color[1], color[2]});
        const float mltp = isMovingRight? 1.0f : -1.0f;
        if(coords == Vector2{0, 3} && i == Player::CLOTHES)
        {
            sprite.setPosition(position + Vector2{-3 * mltp, -35});
        }
        else
        {
            sprite.setPosition(position + Vector2{part.playerPosOffset.x * mltp, part.playerPosOffset.y});
        }    
        sprite.setOrigin(rect.getSize().x / 2.0f, 0);
        window.draw(sprite);
    };

    auto drawHair = [&](int i, Vector2 coords, const float color[3])
    {
        auto &hairs = hairSprites[i];
        sf::IntRect rect{{}, {38, 54}};
        rect.left = coords.x * 38;
        rect.top = coords.y * 54;
        sf::Sprite sprite(hairs, rect);
        if(!isMovingRight)
        {
            sprite.setScale(-1.f * scale.x, 1.f * scale.y);
        }
        sprite.setColor(sf::Color{color[0], color[1], color[2]});
        sprite.setPosition(position + hairOffset);
        sprite.setOrigin(rect.getSize().x / 2.0f, 0);
        window.draw(sprite);
    };

    const float white[3] = {255.0f, 255.0f, 255.0f};
    drawPart(HEAD, {0, headFrame}, character.skinColor);
    drawPart(EYE_WHITE, {0, headFrame}, white);
    drawPart(EYE, {0, headFrame}, character.eyeColor);

    drawHair(character.hairType, {0, hairFrame}, character.hairColor);

    if(canJump)
    {
        drawPart(RIGHT_ARM, {animation.handFrameX, 2}, character.skinColor);
    }
    else
    {
        drawPart(RIGHT_ARM, {2, 3}, character.skinColor); //left arm
    }

    if(character.hasClothes)
    {
        drawPart(CLOTHES, {0, 0}, character.clothesColor);
		// 
		if (canJump)
		{
			drawPart(RIGHT_ARM, {animation.handFrameX, animation.handFrameY}, character.skinColor);
			drawPart(CLOTHES, {0, 3}, character.clothesColor); // shoulder pad
		}
		else
		{
			drawPart(RIGHT_ARM, {2, 1}, character.skinColor);
		}
    }
    else
    {
        drawPart(TORSO, {0 ,0}, character.skinColor);
        if (canJump)
		{
			    drawPart(RIGHT_ARM, {animation.handFrameX, animation.handFrameY}, character.skinColor);
		}
		else
		{
			drawPart(RIGHT_ARM, {2, 1}, character.skinColor);
		}
    }

    if (character.hasPants)
	{
		drawPart(PANTS, {0, headFrame}, character.pantsColor);
	}
	else
	{
		drawPart(LEGS, {0, headFrame}, character.skinColor);
	}
}

void Player::Update(float deltaTime)
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
        isMovingRight = false;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += speed;
        isMovingRight = true;
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

#pragma region handle animation
    if(velocity.x == 0.0f)
    {
        animation.state = PlayerAnimation::STAY;
    }
    else
    {
        animation.state = PlayerAnimation::RUN;
    }
    animation.Update(0.8f, deltaTime);
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
    std::ifstream playerData(filePath);
    if(!playerData.is_open())
    {   
        std::cerr << "Can't open the file: " << filePath << std::endl;
        return;
    }
    std::string line;
    std::getline(playerData, line);
    position = ExtaractVector2FromString(line);
    int i = 0;
    while(std::getline(playerData, line))
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
        inventory[i].SetProperties(std::stoi(itemType), std::stoi(itemCount) );
        i++;
    }
}

Vector2 Player::GetCoords() const
{
    return Round(position / TILE_SIZE);
}

bool Player::IsDead() const
{
    return health <= 0.0f;
}

void Player::loadTextures() 
{
    for(int i = 0; i < Player::BodyParts::BODY_PARTS_COUNT; i++)
    {
        const std::string filename = "resources/player/Player_" + std::to_string(loadedBodyParts[i].textureId) + ".png";
        loadedBodyParts[i].txt.loadFromFile(filename);
    }
    for(int i = 0; i < 20; i++)
    {
        const std::string filename = "resources/player/hair/Player_Hair_" + std::to_string(i + 1) + ".png"; 
        hairSprites[i].loadFromFile(filename);
    }
}