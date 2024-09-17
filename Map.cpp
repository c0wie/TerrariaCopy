#include "Map.hpp"
#include <cmath>
#include <iostream>

bool CollisionSolver(Tile &tile, Player &player, sf::Vector2f &direction)
{
    if(!tile.isSolid)
    {
        return false;
    }
    const sf::Vector2f playerCenter = player.position;
    const sf::Vector2f tileCenter = tile.position;
    const sf::Vector2f playerHalfSize = player.size / 2.0f;
    const sf::Vector2f tileHalfSize = tile.size / 2.0f;
    const sf::Vector2f positionDelta = tileCenter - playerCenter;
    const sf::Vector2f intersection = sf::Vector2f{ fabs(positionDelta.x) - (playerHalfSize.x + tileHalfSize.x), 
                                                    fabs(positionDelta.y) - (playerHalfSize.y + tileHalfSize.y) };
    
    if(intersection.x < 0.0f && intersection.y < 0.0f)
    {
        if(intersection.x > intersection.y)
        {
            if(positionDelta.x > 0.0f)
            {
                player.Move({intersection.x, 0.0f});
                direction = {1.0f, 0.0f};
            }
            else
            {
                player.Move({-intersection.x, 0.0f});
                direction = {-1.0f, 0.0f};
            }
        }
        else
        {   
            if(positionDelta.y > 0.0f)
            {
                player.Move({0.0f, intersection.y});
                direction = {0.0f, 1.0f};
            }
            else
            {
                player.Move({0.0f, -intersection.y});
                direction = {0.0f, -1.0f};
            }
        }
        return true;
    }
    return false;
}

Map::Map() 
{
    for(int i = 0; i < 10; i++)
    {
        tiles.emplace_back(tiles.size(), sf::Vector2f{i * 50.0f, 800.0f}, tileType::GRASS);
        if(i == 5)
        {
            tiles.emplace_back(tiles.size(), sf::Vector2f{i * 50.0f, 750.0f}, tileType::LOG);
        }
    }
}

void Map::Update(float deltaTime)
{
    player.Update(deltaTime);
    sf::Vector2f direction{0.0f, 0.0f};
    
    for(int i = 0; i < tiles.size(); i++)
    {
        if(CollisionSolver(tiles[i], player, direction))
        {
            player.OnCollision(direction);
        }
    }
}

void Map::Draw(sf::RenderWindow &window) const
{
    for(const Tile &tile : tiles)
    {
        tile.Draw(window);
    }
    player.Draw(window);
}