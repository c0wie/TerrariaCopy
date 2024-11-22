#pragma once
#include "Global.hpp"
#include "Vector2.h"
#include <SFML/Graphics.hpp>
#include <memory>

constexpr char TOP_INTERSECTION = 0b1000;
constexpr char LEFT_INTERSECTION = 0b0100;
constexpr char BOTTOM_INTERSECTION = 0b0010;
constexpr char RIGHT_INTERSECTION = 0b0001;

struct TileProperties
{
public:
    TileProperties() {}
    TileProperties(float durability_, int lightLevel_, int lightConsumption_, int textureID, Vector2 atlasSize_) :
        durability(durability_),
        lightLevel(lightLevel_),
        lightConsumption(lightConsumption_),
        txtID(textureID),
        atlasSize(atlasSize_) {}
public:
    sf::Texture txt;
    sf::Color color;
    Vector2 atlasSize{0.0f, 0.0f};
    float durability{0.0f};
    int txtID{0};
    int lightLevel{0};
    int lightConsumption{0};
};

class Tile
{
public:
    enum Type
    {
        NONE = 0,
        STONE,
        GRASS,
        LOG,
        IRON,
        COPPER,
        GOLD,
        SILVER,
        TORCH,
        TREETOP,
        DIRT_WALL,
        BORDER,
        TILE_COUNT
    };
public:
    Tile() = default;
    Tile(Vector2 position, short mainType, short wallType = Tile::NONE);
public:
    void Draw(sf::Font &font, sf::RenderWindow &window) const;
    
    // setters
    void SetProperties(short mainType, short wallType = Tile::NONE);
    void SetLighting(int newLightLevel);
    void SetSubtype(short intersectionInfo);
    void SetPosition(Vector2 newPosition);
    void SetSize(Vector2 newSize);
    void SetDurability(float newDurability);
    void AddDurability(float amount);

    // getters
    std::string GetInfo() const;
    Vector2 GetCoords() const { return m_Position / TILE_SIZE; }
    Vector2 GetPosition() const { return m_Position; }
    Vector2 GetSize() const { return m_Size; }
    int GetLighLevel() const { return m_LightLevel; }
    int GetLightConsumption() const { return std::max(m_LightConsumption, m_WallLightConsumption); }
    float GetDurability() const { return m_Durability; }
    float GetWallDurability() const { return m_WallDurability; }
    short GetType() const { return m_Type; }
    short GetWallType() const { return m_WallType; }

    // queries
    bool IsCollidable() const;
    bool IsNone() const;
    bool HasTexture() const;
    bool IsLightSource() const;
    bool IsWallVisible() const;

    void Load(std::string &line);
    static void LoadTextures();
private:
    sf::Sprite m_Sprite;
    sf::Sprite m_WallSprite;
    Vector2 m_Position{-1.0f, -1.0f};
    Vector2 m_Size{TILE_SIZE, TILE_SIZE};
    Vector2 m_Subtype{0, 0};
    Vector2 m_WallSubtype{-1, -1};
    float m_Durability{-2137.f};
    float m_WallDurability{-2137.f};
    int m_LightConsumption{-2137};
    int m_WallLightConsumption{-2137};
    int m_LightLevel{-2137};
    short m_Type{-1};
    short m_WallType{-1};
};
