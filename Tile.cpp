#include "Tile.hpp"

Tile::Tile(sf::Vector2f position, tileType tileType) : 
    position(position), type(tileType)
{
    if(type == tileType::GRASS)
    {
        isSolid = true;
        color = sf::Color::Green;
    }
    else if(type == tileType::STONE)
    {
        isSolid = true;
        color = sf::Color::Black;
    }
    else if(type == tileType::LOG)
    {
        isSolid = false;
        color = sf::Color::Yellow;
    }
}
void Tile::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape tileBody{size};
    tileBody.setPosition(position);
    tileBody.setOrigin(size / 2.0f);
    tileBody.setFillColor(color);
    window.draw(tileBody);
}