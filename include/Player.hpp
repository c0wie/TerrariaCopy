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
	sf::Color eyeColor{51, 127, 178};
	sf::Color skinColor{236, 161, 130};
	sf::Color hairColor{55, 31, 24};
	sf::Color clothesColor{189, 30, 73};
	sf::Color pantsColor{33, 14, 30};

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
    void Draw(sf::RenderWindow &window) const;
    void Update(float deltaTime, char gameState);
    void Save();
    void Load();
    void PlaceBlock();
    Vector2 GetCoords() const;
public:
    Inventory inventory;
    PlayerAnimation animation;
    PlayerCharacter character;
    Vector2 position{200.0f, 150.0f};
    Vector2 velocity{0.0f, 0.0f};
    Vector2 size{1.76f * TILE_SIZE, 2.7f * TILE_SIZE};
    float jumpHeight{TILE_SIZE * 2.0f};
    float speed{TILE_SIZE * 7.8f};
    float spaceHoldTime{0.1f};
    float canPlaceBlockTimer{PLAYER_PLACE_BLOCK_DELAY};
    float strength{100.0f};
    int health {100};
    bool canJump{false};
    bool canPlaceBlock{true};
    bool isMovingRight{false};
};

void loadPlayerTextures();