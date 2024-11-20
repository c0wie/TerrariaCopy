#include "Tile.hpp"
#include "Math.hpp"
#include <filesystem>

// table with tile information about texture
std::array<TileProperties, Tile::TILE_COUNT> loadedTiles = 
{
    TileProperties{   0.0f,  0, 0, 0, {0, 0}   }, //NONE doesn't have txt
    TileProperties{ 300.0f,  0, 4, 1, {18, 18} }, //STONE
    TileProperties{ 100.0f,  0, 4, 2, {18, 18} }, //GRASS
    TileProperties{ 150.0f,  0, 0, 3, {22, 22} }, //LOG
    TileProperties{ 350.0f,  0, 4, 4, {18, 18} }, //IRON
    TileProperties{ 250.0f,  0, 4, 5, {18, 18} }, //COPPER
    TileProperties{ 150.0f,  0, 4, 6, {18, 18} }, //GOLD
    TileProperties{ 150.0f,  0, 4, 7, {18, 18} }, //SILVER
    TileProperties{ 50.0f , 16, 0, 8, {18, 18} }, //TORCH
    TileProperties{ 150.0f,  0, 0, 9, {82, 82} }, //TREETOP
    TileProperties{ INF,     0, 2, 10, {18, 18} }, // DIRT_WALL
    TileProperties{ INF,     0, 16, 0, {0, 0} }  //BORDER doesn't have txt
};

Tile::Tile(Vector2 position, short mainType, short wallType) :
    m_Position(position)
{
    SetProperties(mainType, wallType);
    m_Sprite.setPosition(position);
    m_WallSprite.setPosition(position);
}

void Tile::Draw(sf::Font &font, sf::RenderWindow &window) const
{
    if(IsWallVisible() && m_WallType != NONE)
    {
        window.draw(m_WallSprite);
    }
    if(HasTexture())
    {
        window.draw(m_Sprite);
    }
    else
    {
        sf::RectangleShape tile(m_Size);
        tile.setOrigin(m_Size / 2.0f);
        tile.setPosition(m_Position);
        if(m_Type == NONE)
        {
            tile.setFillColor(sf::Color::Transparent);
        }
        else if(m_Type == BORDER)
        {
            tile.setFillColor(sf::Color::Magenta);
        }
        else if(m_Type == DIRT_WALL)
        {
            tile.setFillColor(sf::Color::Yellow);
        }
        else
        {
            tile.setFillColor(sf::Color::Black);
        }
        window.draw(tile);
    }
    // sf::Text text(std::to_string(lightLevel), font, 10);
    // text.setPosition(position - Vector2{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f});
    // text.setOutlineThickness(0.6);
    // text.setOutlineColor(sf::Color::Black);
    // text.setColor(sf::Color::White);
    // window.draw(text);
}

void Tile::SetProperties(short mainType, short wallType)
{
    m_Type = mainType;
    m_Durability = loadedTiles[mainType].durability;
    if(IsLightSource())
    {
        m_LightLevel = loadedTiles[mainType].lightLevel;
    }
    m_LightConsumption = loadedTiles[mainType].lightConsumption;
    m_Subtype = {0, 0};
    m_Sprite.setTexture(loadedTiles[mainType].txt);
    m_Sprite.setTextureRect({ m_Subtype * loadedTiles[mainType].atlasSize, loadedTiles[mainType].atlasSize});
    m_Sprite.setOrigin(loadedTiles[mainType].atlasSize / 2.0f);

    m_WallType = wallType;
    m_WallDurability = loadedTiles[wallType].durability;
    if(IsWallVisible())
    {
        m_WallLightConsumption = loadedTiles[wallType].lightConsumption;
    }
    m_WallSubtype = {0, 0};
    m_WallSprite.setTexture(loadedTiles[wallType].txt);
    m_WallSprite.setTextureRect({ m_WallSubtype * loadedTiles[wallType].atlasSize, loadedTiles[wallType].atlasSize});
    m_WallSprite.setOrigin(loadedTiles[wallType].atlasSize / 2.0f);
}

void Tile::SetLighting(int newLightLevel)
{
    newLightLevel = std::min(std::max(0, newLightLevel), 16);
    sf::Color mainColor = loadedTiles[m_Type].color;  
    sf::Color wallColor = loadedTiles[m_WallType].color;     
    
    mainColor.r *= newLightLevel / 16.0f;
    mainColor.g *= newLightLevel / 16.0f;
    mainColor.b *= newLightLevel / 16.0f;

    wallColor.r *= newLightLevel / 16.0f;
    wallColor.g *= newLightLevel / 16.0f;
    wallColor.b *= newLightLevel / 16.0f;

    m_Sprite.setColor(mainColor);
    m_WallSprite.setColor(wallColor);
    m_LightLevel = std::max(0, newLightLevel - std::max(m_LightConsumption, m_WallLightConsumption));
}

void Tile::SetSubtype(short intersectionInfo)
{
    if(!HasTexture())
    {
        return;
    }
    if(m_Type == STONE || m_Type == IRON || m_Type == COPPER || m_Type == SILVER || m_Type == GOLD)
    {
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            m_Subtype = {1, 1};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            m_Subtype = {10, 0};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            m_Subtype = {1, 0};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            m_Subtype = {10, 2};
        }   
        else if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            m_Subtype = {7, 2};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            m_Subtype = {1, 3};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            m_Subtype = {5, 0};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            m_Subtype = {0, 4};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            m_Subtype = {1, 3};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            m_Subtype = {6, 4};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            m_Subtype = {0, 3};
        }
        else if(intersectionInfo & TOP_INTERSECTION)
        {
            m_Subtype = {6, 3};
        }
        else if(intersectionInfo & LEFT_INTERSECTION)
        {
            m_Subtype = {9, 0};
        }
        else if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            m_Subtype = {7, 0};
        }
        else if(intersectionInfo & RIGHT_INTERSECTION)
        {
            m_Subtype = {12, 0};
        }
        else if(intersectionInfo == 0)
        {
            m_Subtype = {9, 3};
        }
    }
    else if(m_Type == GRASS)
    {
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            m_Subtype = {1, 1};
        }
        else if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            m_Subtype = {7, 2};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            // m_Subtype = {11, 0};
            m_Subtype = {1, 1};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            m_Subtype = {1, 0};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            // m_Subtype = {10, 2};
            m_Subtype = {1, 1};
        }   
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            m_Subtype = {1, 4};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            m_Subtype = {1, 1};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            m_Subtype = {0, 4};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            m_Subtype = {1, 3};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            // m_Subtype = {6, 4};
            m_Subtype = {1, 1};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            m_Subtype = {2, 3};
        }
        else if(intersectionInfo & TOP_INTERSECTION)
        {
            m_Subtype = {6, 3};
        }
        else if(intersectionInfo & LEFT_INTERSECTION)
        {
            m_Subtype = {0, 13};
        }
        else if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            m_Subtype = {6, 5};
        }
        else if(intersectionInfo & RIGHT_INTERSECTION)
        {
            m_Subtype = {3, 13};
        }
        else if(intersectionInfo == 0)
        {
            m_Subtype = {9, 3};
        }
    }
    else if(IsLightSource())
    {
        m_Subtype = {0, 0};
    }
    m_Sprite.setTextureRect({m_Subtype * loadedTiles[m_Type].atlasSize, loadedTiles[m_Type].atlasSize});
}

void Tile::SetPosition(Vector2 newPosition)
{
    m_Position = newPosition;
}

void Tile::SetSize(Vector2 newSize)
{
    m_Size = newSize;
}

void Tile::SetDurability(float newDurability)
{
    m_Durability = std::min(std::max(0.0f, newDurability), loadedTiles[m_Type].durability);
}

void Tile::AddDurability(float amount)
{
    m_Durability = std::min(std::max(0.0f, m_Durability + amount), loadedTiles[m_Type].durability);
}

std::string Tile::GetInfo() const
{
    return std::string(m_Position.GetString() + ',' + std::to_string(m_Type) + ',' + m_Subtype.GetString());
}

bool Tile::IsCollidable() const
{
    return m_Type != NONE && m_Type != LOG && m_Type != TREETOP && m_Type != TORCH;
}

bool Tile::IsNone() const
{
    return m_Type == NONE;
}

bool Tile::HasTexture() const
{
    return m_Type != NONE && m_Type != BORDER;
}

bool Tile::IsLightSource() const
{
    return m_Type == TORCH;
}

bool Tile::IsWallVisible() const
{
    return m_Type == NONE || m_Type == TORCH;
}

void Tile::Load(std::string &line)
{
    const int commaPosition1 = line.find(',');
    if(commaPosition1 == std::string::npos)
    {
        std::cout << line << '\n';
        std::cerr << "Invalid format: no commma1 found.\n";
        std::exit(1);
    }
    const std::string tilePosition = line.substr(0, commaPosition1);
    const int commaPosition2 = line.find(',', commaPosition1 + 1);
    if(commaPosition2 == std::string::npos)
    {
        std::cout << line << '\n';
        std::cerr << "Invalid format: no commma2 found.\n";
        std::exit(1);
    }
    const std::string itemType = line.substr(commaPosition1 + 1, commaPosition2 - commaPosition1 - 1);
    const std::string subtype = line.substr(commaPosition2 + 1);
    
    m_Subtype = ExtaractVector2FromString(subtype);
    m_Position = ExtaractVector2FromString(tilePosition);
    SetProperties(std::stoi(itemType));
    m_Sprite.setPosition(m_Position);
}

void Tile::LoadTextures()
{
    for(int i = 0; i < Tile::TILE_COUNT; i++)
    {
        if(loadedTiles[i].txtID == 0)
        {
            continue;
        }
        const std::string filename = "resources/Tiles/Tiles_" + std::to_string(loadedTiles[i].txtID) + ".png";
        loadedTiles[i].txt.loadFromFile(filename);
        sf::Sprite sp(loadedTiles[i].txt);
        loadedTiles[i].color = sp.getColor();
    }
}