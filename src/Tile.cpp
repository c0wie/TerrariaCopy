#include "Tile.hpp"
#include "Math.hpp"
#include <filesystem>


Tile::Tile(Vector2 Position, short TileType) :
    position(Position),
    type(TileType)
{
    SetTileProperties(type);
}

void Tile::Draw(sf::RenderWindow &window) const
{
    if(HasTexture())
    {
        const Vector2 txtCount = texturesCountInFile[type - 1];
        const sf::Vector2u fileSize = txt->getSize();

        sf::IntRect rect;
        rect.width = fileSize.x / txtCount.x;
        rect.height = fileSize.y / txtCount.y;
        rect.top = subtype.y * rect.height;
        rect.left = subtype.x * rect.width;

        sf::Sprite tile;
        tile.setOrigin(rect.width / 2.0f, rect.height / 2.0f);
        tile.setPosition(position);
        tile.setTexture(*txt);
        tile.setTextureRect(rect);
        window.draw(tile);
    }
    else
    {
        sf::RectangleShape tile;
        tile.setSize(size);
        tile.setOrigin(size / 2.0f);
        tile.setPosition(position);
        tile.setFillColor(color);
        window.draw(tile);
    }
}

void Tile::SetTileProperties(short Type)
{
    auto tp = tilePropertiesTable[Type];
    type = Type;
    color = tp.first;
    durability = tp.second;
    LoadTexture();
}

void Tile::LoadTexture()
{
    if(HasTexture())
    {
        if( !txt->loadFromFile("resources/Tiles_" + std::to_string(type) + ".png") )
        {
            std::cout << "Unable to load texture from file" << "Tiles_" + std::to_string(type) + ".png\n"; 
            std::exit(1);
        }
    }
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
    SetTileProperties(std::stoi(itemType));
    position = ExtaractVector2FromString(line);
    this->subtype = ExtaractVector2FromString(subtype);
}

std::string Tile::GetInfo() const
{
    return std::string(position.GetString() + ',' + std::to_string(type) + ',' + subtype.GetString());
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

std::array<std::pair<sf::Color, float>, Tile::Type::TILE_COUNT> Tile::tilePropertiesTable =
{
    std::pair(sf::Color::Transparent, 0.0f),
    std::pair(sf::Color{111, 118, 130}, 300.0f),
    std::pair(sf::Color::Green, 100.0f),
    std::pair(sf::Color::Yellow, 150.0f),
    std::pair(sf::Color{164, 52, 235}, INF),
    std::pair(sf::Color::Green, 350.0f),
    std::pair(sf::Color::Green, 250.0f),
    std::pair(sf::Color::Green, 150.0f),
    std::pair(sf::Color::Green, 150.0f)
};