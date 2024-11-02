#include "Tile.hpp"
#include "Math.hpp"
#include <filesystem>

// table with tile information about texture
std::array<TileTexture, Tile::TILE_COUNT> loadedTiles = 
{
    TileTexture{ 0, {0, 0}   }, //NONE doesn't have txt
    TileTexture{ 1, {18, 18} }, //STONE
    TileTexture{ 2, {18, 18} }, //GRASS
    TileTexture{ 3, {22, 22} }, //LOG
    TileTexture{ 4, {18, 18} }, //IRON
    TileTexture{ 5, {18, 18} }, // COPPER
    TileTexture{ 6, {18, 18} }, // GOLD
    TileTexture{ 7, {18, 18} }, // SILVER
    TileTexture{ 8, {82, 82} }, // TREETOP
    TileTexture{ 9, {0, 0}   }  // BORDER doesn't have txt
};

// table with information about tile other properties for now it is just durablility
std::array<float, Tile::TILE_COUNT> durablilityTable = 
{
    0.0f,
    300.0f,
    100.0f,
    150.0f,
    350.0f,
    250.0f,
    150.0f,
    150.0f,
    150.0f,
    INF
};

Tile::Tile(Vector2 Position, short TileType) :
    position(Position)
{
    SetProperties(TileType);
    sprite->setPosition(position);
}

void Tile::Draw(sf::RenderWindow &window) const
{
    if(HasTexture())
    {
        window.draw(*sprite);
    }
    else
    {
        sf::RectangleShape tile;
        tile.setSize(size);
        tile.setOrigin(size / 2.0f);
        if(type == NONE)
        {
            tile.setFillColor(sf::Color::Transparent);
        }
        else
        {
            tile.setFillColor(sf::Color::Magenta);
        }
        tile.setPosition(position);
        window.draw(tile);
    }
}

void Tile::SetProperties(short Type)
{
    type = Type;
    durability = durablilityTable[Type];
    sprite->setTexture(loadedTiles[Type].txt);
    sprite->setTextureRect({{subtype.x * loadedTiles[Type].atlasSize.x, subtype.y * loadedTiles[Type].atlasSize.y}, loadedTiles[Type].atlasSize});
    sprite->setOrigin(loadedTiles[1].atlasSize / 2.0f);
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
    const std::string coords = line.substr(0, commaPosition1);
    const int commaPosition2 = line.find(',', commaPosition1 + 1);
    if(commaPosition2 == std::string::npos)
    {
        std::cout << line << '\n';
        std::cerr << "Invalid format: no commma2 found.\n";
        std::exit(1);
    }
    const std::string itemType = line.substr(commaPosition1 + 1, commaPosition2 - commaPosition1 - 1);
    const std::string subtype = line.substr(commaPosition2 + 1);
    SetProperties(std::stoi(itemType));
    position = ExtaractVector2FromString(line);
    this->subtype = ExtaractVector2FromString(subtype);
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
        else if(intersectionInfo == 0)
        {
            subtype = {9, 3};
        }
        else if(intersectionInfo & TOP_INTERSECTION)
        {
            subtype = {6, 0};
        }
        else if(intersectionInfo & LEFT_INTERSECTION)
        {
            subtype = {9, 0};
        }
        else if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            subtype = {7, 3};
        }
        else if(intersectionInfo & RIGHT_INTERSECTION)
        {
            subtype = {12, 0};
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
            subtype = {11, 0};
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
        else if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {1, 4};
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
    return type != NONE && type != LOG && type != TREETOP;
}

bool Tile::isNone() const
{
    return type == NONE;
}

bool Tile::HasTexture() const
{
    return type != NONE && type != BORDER;
}

void loadTileTextures()
{
    for(int i = 0; i < Tile::TILE_COUNT; i++)
    {
        if(i == 0 || i == 9)
        {
            continue;
        }
        const std::string filename = "resources/Tiles/Tiles_" + std::to_string(loadedTiles[i].txtID) + ".png";
        loadedTiles[i].txt.loadFromFile(filename);
    }
}