#include "Map.hpp"
#include "Collision.hpp"
#include "Math.hpp"
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;

Map::Map(bool generateMap) :
    tiles()
{
    if(generateMap)
    {
        populateMap(PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 1.0f), 5, 0.75f), tiles);
    }
    else
    {
        loadMap(tiles);
    }
}

Map::~Map()
{
    saveMap(tiles);
}

void Map::Update(float deltaTime)
{
    player.Update(deltaTime);
    Vector2 cp, cn;
    float ct = 0.0f;
    std::vector<std::pair<Vector2, float>> z;

// retrive collison tiles
    std::array<Vector2, 12> possibleCollisions = FindPossibleCollisionTileCoords(player.position, player.size);
    for(int i = 0; i < possibleCollisions.size(); i++)
    {
        const int x = possibleCollisions[i].x;
        const int y = possibleCollisions[i].y;
        if(!tiles[y * MAP_WIDTH + x].isSolid)
        {
            continue;
        }
        if(RectDynamicRectCollision(player, tiles[y * MAP_WIDTH + x], cp, cn, ct, deltaTime))
        {
            z.emplace_back(possibleCollisions[i], 0.0f);
        }
    }
// sort collision tiles in the smallest map id order 
    std::sort(z.begin(), z.end(), [](const std::pair<Vector2, float> &a, const std::pair<Vector2, float> &b){
        return a.second < b.second;
    });

// resolve player collision
    for(auto j : z)
    {
        if(RectDynamicRectCollision(player, tiles[j.first.y * MAP_WIDTH + j.first.x], cp, cn, ct, deltaTime))
        {
            player.velocity += cn * Abs(player.velocity) * (1.0f - ct);
            if(cn == Vector2{0.0f, -1.0f})
            {
                player.canJump = true;
            }
        }
    }
    player.Move(player.velocity * deltaTime);
    GetTilesToDraw(player.position);
}

void Map::Draw(sf::RenderWindow &window) const
{
    auto tilesToDraw = GetTilesToDraw(player.position);
    for(int i = 0; i < tilesToDraw.size(); i++)
    {
        const int index = tilesToDraw[i];
        if(tiles[index].type == TileType::AIR)
        {
            continue;
        }
        tiles[tilesToDraw[i]].Draw(window);
    }

    // shows tested area for collision resolution
    
    // auto cp = FindPossibleCollisionTileCoords(player.position, player.size);
    // for(auto elem : cp)
    // {
    //     Tile t = tiles[elem.y * MAP_WIDTH + elem.x];
    //     t.color = sf::Color::Cyan;
    //     t.Draw(window);
    // }

    player.Draw(window);
}

std::array<Vector2, 12> Map::FindPossibleCollisionTileCoords(Vector2 position, Vector2 size) const
{
    int i = 0;
    std::array<Vector2, 12>  CollideTiles;
    const Vector2 topLeftCorner = Floor((position - size / 2.0f) / TILE_SIZE);
    const Vector2 botRightCorner = Ceil((position + size / 2.0f) / TILE_SIZE);
    for(int y = topLeftCorner.y; y <= botRightCorner.y; y++)
    {
        for(int x = topLeftCorner.x; x <= botRightCorner.x; x++)
        {
            CollideTiles[i] = Vector2(x, y);
            i++;
        }
    }
    return CollideTiles;
}

Tile decodeTileInfo(std::string &line)
{
    Tile tile;
    int commaPosition = line.find(',');
    if(commaPosition == std::string::npos)
    {
        std::cout << line << '\n';
        std::cerr << "Invalid format: no commma found.\n";
        std::exit(1);
    }
    std::string coords = line.substr(0, commaPosition);
    std::string tiletype = line.substr(commaPosition + 1);
    tile.setTileProperties(static_cast<TileType>(std::stoi(tiletype)));

    int semicolonPos = coords.find(';');
    if(semicolonPos == std::string::npos)
    {
        std::cerr << "Invalid format: no semicolon found.\n";
        std::exit(1);
    }

    std::string xStr = coords.substr(0, semicolonPos);
    std::string yStr = coords.substr(semicolonPos + 1);
    tile.position.x = std::stoi(xStr);  // Convert x position to an integer
    tile.position.y = std::stoi(yStr);  // Convert y position to an integer
    return tile;
}

void saveMap(const std::array<Tile, MAP_WIDTH * MAP_HEIGHT> &map)
{
    fs::path filePath = "SAVE_MAP.csv";
    if(!fs::exists(filePath))
    {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return;
    }
    std::ofstream mapData(filePath);
    if(!mapData.is_open())
    {   
        std::cerr << "Can't open the file: " << filePath << std::endl;
        return;
    }

    // saving is from left to right, not top to bottam as my acceses to map
    for(int i = 0; i < map.size(); i++)
    {
        mapData << map[i].position.x << ';' << map[i].position.y << ',' << (int)map[i].type << '\n';
    }
    mapData.close();
}

void loadMap(std::array<Tile, MAP_WIDTH * MAP_HEIGHT> &map)
{
    fs::path filePath = "SAVE_MAP.csv";
    if(!fs::exists(filePath))
    {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return;
    }
    std::ifstream mapData(filePath);
    if(!mapData.is_open())
    {   
        std::cerr << "Can't open the file: " << filePath << std::endl;
        return;
    }
    std::string line;
    int i = 0;
    while(std::getline(mapData, line))
    {
        if(line.empty())
        {
            continue;
        }

        map[i] = decodeTileInfo(line);
        i++;
    }
}

void populateMap(const std::array<float, MAP_WIDTH> &mapSketch, std::array<Tile, MAP_WIDTH * MAP_HEIGHT> &map)
{
    //creates land
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        int y = (int)(mapSketch[x] * MAP_HEIGHT);
        for(int i = MAP_HEIGHT - 1; i >= y; i--)
        {
            TileType type = TileType::GRASS;
            if((float)i > MAP_HEIGHT * 0.7f)
            {
                type = TileType::STONE;
            }
            map[i * MAP_WIDTH + x] = Tile(Vector2{x * TILE_SIZE, i * TILE_SIZE}, type);
        }
    }
    // creates border
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        // Top border
        map[i] = Tile(Vector2{i * TILE_SIZE, 0}, TileType::BORDER);

        // Bottom border
        map[(MAP_HEIGHT - 1) * MAP_WIDTH + i] = Tile(Vector2{i * TILE_SIZE, (MAP_HEIGHT - 1) * TILE_SIZE}, TileType::BORDER);
    }

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        // Left border
        map[MAP_WIDTH * i] = Tile(Vector2{0, i * TILE_SIZE}, TileType::BORDER);

        // Right border
        map[(i + 1) * MAP_WIDTH - 1] = Tile(Vector2{(MAP_WIDTH - 1) * TILE_SIZE, i * TILE_SIZE}, TileType::BORDER);
    }
}

std::vector<int> GetTilesToDraw(Vector2 playerPosition)
{
    std::vector<int> tilesToDraw;
    constexpr Vector2 halfScreenSize = Vector2{SCREEN_WIDTH / 2.0f + 100.0f, SCREEN_HEIGHT / 2.0f + 100.0f};
    Vector2 topLeftCorner = Vector2{playerPosition.x - halfScreenSize.x, playerPosition.y + halfScreenSize.y};
    Vector2 botRightCorner = Vector2{playerPosition.x + halfScreenSize.x, playerPosition.y - halfScreenSize.y};
    Vector2 topLeftTile = Ceil(topLeftCorner / TILE_SIZE);
    Vector2 botRightTile = Ceil(botRightCorner / TILE_SIZE);
    int i = 0;
    if(topLeftTile.x < 0.0f)
    {
        topLeftTile.x = 0.0f;
    }
    if(topLeftTile.y < 0.0f)
    {
        topLeftTile.y = 0.0f;
    }
    if(botRightTile.x < 0.0f)
    {
        botRightTile.x = 0.0f;
    }
    if(botRightTile.y < 0.0f)
    {
        botRightTile.y = 0.0f;
    }
    if(topLeftTile.x > MAP_WIDTH)
    {
        topLeftTile.x = MAP_WIDTH;
    }
    if(topLeftTile.y > MAP_HEIGHT)
    {
        topLeftTile.y = MAP_HEIGHT;
    }
    if(botRightTile.x > MAP_WIDTH)
    {
        botRightTile.x = MAP_WIDTH;
    }
    if(botRightTile.y > MAP_HEIGHT)
    {
        botRightTile.y = MAP_HEIGHT;
    }
    for(int x = topLeftTile.x; x < botRightTile.x; x++)
    {
        for(int y = botRightTile.y; y < topLeftTile.y; y++)
        {
            tilesToDraw.push_back(y * MAP_WIDTH + x);
        }
    }
    return tilesToDraw;
}