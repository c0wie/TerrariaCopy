#include "Tile.hpp"
#include "Math.hpp"
#include <filesystem>


Tile::Tile(Vector2 Position, short TileType) :
    position(Position),
    type(TileType),
    txt(std::make_shared<sf::Texture>())
{
    SetTileProperties(type);
}

void Tile::Draw(sf::RenderWindow &window) const
{
    if(type != NONE && type != BORDER)
    {
        sf::IntRect rect;
        rect.width = txt->getSize().x / fileCoords[type - 1].x;
        rect.height = txt->getSize().y / fileCoords[type - 1].y;
        rect.top = subtype.y * rect.height;
        rect.left = subtype.x * rect.width;

        sf::RectangleShape tile;
        tile.setSize(size);
        tile.setOrigin(size / 2.0f);
        tile.setPosition(position);
        tile.setTexture(txt.get());
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
    if(type == STONE || type == GRASS)
    {
        txt->loadFromFile("resources/Tiles_" + std::to_string(type) + ".png");
    }
}

void Tile::UpdateTextureRect(short intersectionInfo)
{
    if(type == STONE)
    {
        if(intersectionInfo == 0)
        {
            subtype = {9, 3};
        }
        if(intersectionInfo & TOP_INTERSECTION)
        {
            subtype = {6, 0};
        }
        if(intersectionInfo & LEFT_INTERSECTION)
        {
            subtype = {9, 0};
        }
        if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            subtype = {7, 3};
        }
        if(intersectionInfo & RIGHT_INTERSECTION)
        {
            subtype = {12, 0};
        }
        if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {1, 3};
        }
        if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {5, 0};
        }
        if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 4};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {1, 3};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {6, 4};
        }
        if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 3};
        }
        if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {10, 0};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            subtype = {1, 0};
        }
        if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            subtype = {10, 2};
        }   
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {7, 2};
        }
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {1, 1};
        }
    }
    else if(type == GRASS)
    {
        if(intersectionInfo == 0)
        {
            subtype = {9, 3};
        }
        if(intersectionInfo & TOP_INTERSECTION)
        {
            subtype = {6, 0};
        }
        if(intersectionInfo & LEFT_INTERSECTION)
        {
            subtype = {9, 0};
        }
        if(intersectionInfo & BOTTOM_INTERSECTION)
        {
            subtype = {7, 3};
        }
        if(intersectionInfo & RIGHT_INTERSECTION)
        {
            subtype = {12, 0};
        }
        if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION)) == (TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {1, 3};
        }
        if((intersectionInfo & (TOP_INTERSECTION | BOTTOM_INTERSECTION)) == (TOP_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {5, 0};
        }
        if((intersectionInfo & (TOP_INTERSECTION | RIGHT_INTERSECTION)) == (TOP_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 4};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION)) == (LEFT_INTERSECTION | BOTTOM_INTERSECTION) )
        {
            subtype = {1, 3};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | RIGHT_INTERSECTION)) == (LEFT_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {6, 4};
        }
        if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == (BOTTOM_INTERSECTION | RIGHT_INTERSECTION) )
        {
            subtype = {0, 3};
        }
        if((intersectionInfo & (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {11, 0};
        }
        if((intersectionInfo & (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION)) == 
            (LEFT_INTERSECTION | BOTTOM_INTERSECTION | RIGHT_INTERSECTION))
        {
            subtype = {1, 0};
        }
        if((intersectionInfo & (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION)) ==
            (BOTTOM_INTERSECTION | RIGHT_INTERSECTION | TOP_INTERSECTION))
        {
            subtype = {10, 2};
        }   
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION))
        {
            subtype = {7, 2};
        }
        if((intersectionInfo & (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION)) ==
            (RIGHT_INTERSECTION | TOP_INTERSECTION | LEFT_INTERSECTION | BOTTOM_INTERSECTION))
        {
            subtype = {1, 1};
        }
    }
}

bool Tile::isCollidable() const
{
    return type != NONE && type != LOG;
}

bool Tile::isNone() const
{
    return type == NONE;
}

std::array<std::pair<sf::Color, float>, Tile::Type::TILE_COUNT> Tile::tilePropertiesTable =
{
    std::pair(sf::Color::Transparent, 0.0f),
    std::pair(sf::Color{111, 118, 130}, 300.0f),
    std::pair(sf::Color::Green, 100.0f),
    std::pair(sf::Color::Yellow, 150.0f),
    std::pair(sf::Color{164, 52, 235}, INF),
};