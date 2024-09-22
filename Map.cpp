#include "Map.hpp"
#include "Collision.hpp"
#include <iostream>
#include <cmath>
#include <vector>


void populateMap(std::array<std::array<Tile, MAP_WIDTH>, MAP_HEIGHT> &map)
{
    for(int y = 0; y < mapSketch.size(); y++)
    {
        for(int x = 0; x < mapSketch[y].size(); x++)
        {
            Tile &cell = map[y][x];
            cell.position = Vector2(x * TILE_SIZE, y * TILE_SIZE);
            switch (mapSketch[y][x])
            {
            case '0':
                cell.setTileProperties(tileType::AIR);
                break;
            case 'g':
                cell.setTileProperties(tileType::GRASS);
                break;
            case 'l':
                cell.setTileProperties(tileType::LOG);
                break;
            case 's':
                cell.setTileProperties(tileType::STONE);
                break;
            default:
                std::cerr << "Undefined character in map\n";
                std::exit(1);
            }
        }
    }
}

Map::Map() :
    tiles()
{
    populateMap(tiles);
}

void Map::Update(float deltaTime)
{
    player.Update(deltaTime);
    Vector2 cp, cn;
    float ct = 0.0f;
    std::vector<std::pair<Vector2, float>> z;

    for(int y = 0; y < tiles.size(); y++)
    {
        for(int x = 0; x < tiles[y].size(); x++)
        {
            if(!tiles[y][x].isSolid)
            {
                continue;
            }
            if(RectDynamicRectCollision(player, tiles[y][x], cp, cn, ct, deltaTime))
            {
                z.emplace_back(Vector2{x, y}, ct);
            }
        }
    }

    std::sort(z.begin(), z.end(), [](const std::pair<Vector2, float> &a, const std::pair<Vector2, float> &b){
        return a.second < b.second;
    });

    for(auto j : z)
    {
        if(RectDynamicRectCollision(player, tiles[j.first.y][j.first.x], cp, cn, ct, deltaTime))
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
    for(const auto &row : tiles)
    {
        for(const auto &tile : row)
        {
            tile.Draw(window);
        }
    }
    player.Draw(window);
}