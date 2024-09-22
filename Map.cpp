#include "Map.hpp"
#include "Collision.hpp"
#include <iostream>
#include <cmath>
#include <vector>

bool CollisionSolver(Tile &tile, Player &player, Vector2 &direction)
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
    const sf::Vector2f intersection = sf::Vector2f{ std::abs(positionDelta.x) - (playerHalfSize.x + tileHalfSize.x), 
                                                    std::abs(positionDelta.y) - (playerHalfSize.y + tileHalfSize.y) };
    
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

void populateMap(std::array<Tile, TILES_COUNT> &map, int rows)
{
    for(int j = 0; j < rows; j++)
    {
        const float y = 800.0f - 50.0f * j;
        for(int i = 0; i < TILES_COUNT / rows; i++)
        {
            const float x = i * 50.0f;
            const int ID = TILES_COUNT / rows * j + i;
            map[ID] = Tile(ID, {x, y}, tileType::GRASS);
        }
    }
}

Map::Map() :
    tiles()
{
    populateMap(tiles, 1);
    
}

void Map::Update(float deltaTime)
{
    player.Update(deltaTime);
    Vector2 cp, cn;
    float ct = 0.0f;
    std::vector<std::pair<int, float>> z;

    for(int i = 0; i < tiles.size(); i++)
    {
        if(RectDynamicRectCollision(player, tiles[i], cp, cn, ct, deltaTime))
        {
            z.emplace_back(i, ct);
        }
    }

    std::sort(z.begin(), z.end(), [](const std::pair<int, float> &a, const std::pair<int, float> &b){
        return a.second < b.second;
    });

    for(auto j : z)
    {
        if(RectDynamicRectCollision(player, tiles[j.first], cp, cn, ct, deltaTime))
        {
            
            player.velocity += cn * abs(player.velocity) * (1.0f - ct);
            if(cn == Vector2{0.0f, -1.0f})
            {
                player.canJump = true;
            }
        }
    }
    player.Move(player.velocity * deltaTime);
}

void Map::Draw(sf::RenderWindow &window) const
{
    for(const Tile &tile : tiles)
    {
        tile.Draw(window);
    }
    player.Draw(window);
}