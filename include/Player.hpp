#pragma once
#include "Global.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics.hpp>

constexpr float PLAYER_PLACE_BLOCK_DELAY = 0.21;
constexpr float DAMAGE_TRESHOLD_SPEED = 500.0f;
constexpr float MAX_SPACE_HOLD_TIME = 0.15f;

struct PlayerAnimation
{
	enum States
	{
		STAY,
		RUN
	};

	float timer{0.0f};
	int state{STAY};
	int headFrame{0};
	int hairFrame{0};
	int handFrameX{2};
	int handFrameY{0};
	int shoulderPadFrame{0};
	bool isFrameUp{false};
	bool grounded{true};

	void Update(float runSpeed, float deltaTime);
};
struct BodyPart
{
	BodyPart() {};

	BodyPart(int textureId, Vector2 playerPosOffset, Vector2 atlasSize) : 
        textureId(textureId),
        playerPosOffset(playerPosOffset),
        atlasSize(atlasSize) {};

	int textureId{0};
    Vector2 playerPosOffset{0.0f, 0.0f};
	Vector2 atlasSize{0, 0};
	sf::Texture txt;
};
struct PlayerCharacter
{
	float eyeColor[3] = {51, 127, 178};
	float skinColor[3] = {236, 161, 130};
	float hairColor[3] = {55, 31, 24};
	float clothesColor[3] = {189, 30, 73};
	float pantsColor[3] = {33, 14, 30};

	int hairType{0};
	bool hasClothes{true};
	bool hasPants{true};
};

class Player
{
public:
    enum BodyParts
    {
        HEAD,
        EYE_WHITE,
        EYE,
        TORSO,
        RIGHT_ARM,
        LEGS,
        CLOTHES,
        PANTS,

        BODY_PARTS_COUNT
    };
public:
    Player() = default;
    void Move(Vector2 offset) { position += offset; }
    void Draw(Vector2 scale, sf::RenderWindow &window) const;
    void Update(float deltaTime);
    void Save();
    void Load();
    Vector2 GetCoords() const;
    bool IsDead() const;
    static void loadTextures();
public:
    Inventory inventory;
    PlayerAnimation animation;
    PlayerCharacter character;
    Vector2 position{SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{1.76f * TILE_SIZE, 2.7f * TILE_SIZE};
    float jumpHeight{TILE_SIZE * 2.0f};
    float speed{TILE_SIZE * 7.8f};
    float spaceHoldTime{0.1f};
    float canPlaceBlockTimer{PLAYER_PLACE_BLOCK_DELAY};
    float strength{100.0f};
    int health {100};
    bool canJump{true};
    bool canPlaceBlock{true};
    bool isMovingRight{false};
};
