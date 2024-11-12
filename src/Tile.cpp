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
    TileProperties{ 50.0f , 16, 0, 8, {0, 0}   }, //TORCH
    TileProperties{ 150.0f,  0, 0, 9, {82, 82} }, //TREETOP
    TileProperties{ INF,    0, 16, 10, {0, 0}   }  //BORDER doesn't have txt
};

Tile::Tile(Vector2 Position, short TileType) :
    position(Position)
{
    SetProperties(TileType);
    sprite->setPosition(position);
}

void Tile::Draw(sf::Font &font, sf::RenderWindow &window) const
{
    if(HasTexture())
    {
        window.draw(*sprite);
    }
    else
    {
        sf::RectangleShape tile(size);
        tile.setOrigin(size / 2.0f);
        tile.setPosition(position);
        if(type == NONE)
        {
            tile.setFillColor(sf::Color::Transparent);
        }
        else if(type == BORDER)
        {
            tile.setFillColor(sf::Color::Magenta);
        }
        else if(type == TORCH)
        {
            tile.setFillColor(sf::Color::Yellow);
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

void Tile::SetProperties(short Type)
{
    if(sprite == nullptr)
    {
        sprite = std::make_unique<sf::Sprite>();
    }
    const TileProperties tileProperties = loadedTiles[Type];
    durability = tileProperties.durability;
    if(IsLightSource() && Type != TORCH)
    {
        lightLevel = loadedTiles[Type].lightLevel;
    }
    type = Type;
    // light sources always has the same light level and other types of blocks not
    if(IsLightSource())
    {
        lightLevel = loadedTiles[Type].lightLevel;
    }
    lightConsumption = tileProperties.lightConsumption;
    sprite->setTexture(loadedTiles[Type].txt);
    sprite->setTextureRect({{subtype.x * tileProperties.atlasSize.x, subtype.y * tileProperties.atlasSize.y}, tileProperties.atlasSize});
    if(Type == TREETOP)
    {
        sprite->setOrigin({40, 40});
    }
    else
    {
        sprite->setOrigin(loadedTiles[1].atlasSize / 2.0f);
    }
}

void Tile::SetLighting(int newLightLevel)
{
    if(IsLightSource())
    {
        return;
    }
    newLightLevel = std::min(std::max(0, newLightLevel), 16);
    sf::Sprite temp(loadedTiles[type].txt);
    sf::Color newColor = temp.getColor();
    newColor.r *= newLightLevel / 16.0f;
    newColor.g *= newLightLevel / 16.0f;
    newColor.b *= newLightLevel / 16.0f;
    sprite->setColor(newColor);
    lightLevel = std::max(0, newLightLevel - lightConsumption);
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
    
    this->subtype = ExtaractVector2FromString(subtype);
    position = ExtaractVector2FromString(tilePosition);
    SetProperties(std::stoi(itemType));
    sprite->setPosition(position);
}

std::string Tile::GetInfo() const
{
    return std::string(position.GetString() + ',' + std::to_string(type) + ',' + subtype.GetString());
}

Vector2 Tile::GetCoords() const
{
    return position / TILE_SIZE;
}

void Tile::UpdateTextureRect(short intersectionInfo)
{
    if(!HasTexture())
    {
        return;
    }
    if(type == STONE || type == IRON || type == COPPER || type == SILVER || type == GOLD)
    {
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {1, 1};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {10, 0};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            subtype = {1, 0};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            subtype = {10, 2};
        }   
        else if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {7, 2};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {1, 3};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {5, 0};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 4};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {1, 3};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {6, 4};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 3};
        }
        else if(intersectionInfo & TOP_INTERSECTION)
        {
            subtype = {6, 3};
        }
        else if(intersectionInfo & LEFT_INTERSECTION)
        {
            subtype = {9, 0};
        }
        else if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            subtype = {7, 0};
        }
        else if(intersectionInfo & RIGHT_INTERSECTION)
        {
            subtype = {12, 0};
        }
        else if(intersectionInfo == 0)
        {
            subtype = {9, 3};
        }
    }
    else if(type == GRASS)
    {
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {1, 1};
        }
        else if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {7, 2};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            // subtype = {11, 0};
            subtype = {1, 1};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            subtype = {1, 0};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            // subtype = {10, 2};
            subtype = {1, 1};
        }   
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {1, 4};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {1, 1};
        }
        else if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 4};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {1, 3};
        }
        else if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            // subtype = {6, 4};
            subtype = {1, 1};
        }
        else if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {2, 3};
        }
        else if(intersectionInfo & TOP_INTERSECTION)
        {
            subtype = {6, 3};
        }
        else if(intersectionInfo & LEFT_INTERSECTION)
        {
            subtype = {0, 13};
        }
        else if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            subtype = {6, 5};
        }
        else if(intersectionInfo & RIGHT_INTERSECTION)
        {
            subtype = {3, 13};
        }
        else if(intersectionInfo == 0)
        {
            subtype = {9, 3};
        }
    }
    sprite->setTextureRect({{subtype.x * loadedTiles[type].atlasSize.x, subtype.y * loadedTiles[type].atlasSize.y}, loadedTiles[type].atlasSize});
}

bool Tile::isCollidable() const
{
    return type != NONE && type != LOG && type != TREETOP && type != TORCH;
}

bool Tile::isNone() const
{
    return type == NONE;
}

bool Tile::HasTexture() const
{
    return type != NONE && type != BORDER && type != TORCH;
}

bool Tile::IsLightSource() const
{
    return type == TORCH;
}

void Tile::loadTextures()
{
    for(int i = 0; i < Tile::TILE_COUNT; i++)
    {
        if(i == NONE || i == TORCH || i == BORDER)
        {
            continue;
        }
        const std::string filename = "resources/Tiles/Tiles_" + std::to_string(loadedTiles[i].txtID) + ".png";
        loadedTiles[i].txt.loadFromFile(filename);
    }
}