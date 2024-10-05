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
    tiles(),
    player(generateMap)
{
    if(generateMap)
    {
        Generate(PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 1.0f), 5, 0.75f));
    }
    else
    {
        Load();
    }
}

Map::~Map()
{
    Save();
}

void Map::Update(Vector2 mousePos, bool isRelased, float deltaTime)
{
    HandleMouseInput(mousePos, deltaTime);
    player.Update(deltaTime);
    HandleCollision(deltaTime);
    player.Move(player.velocity * deltaTime);
}

void Map::Draw(sf::RenderWindow &window) const
{
    auto tilesToDraw = GetTilesToDraw(player.position);
    for(int i = 0; i < tilesToDraw.size(); i++)
    {
        const int index = tilesToDraw[i];
        if(tiles[index].type == ItemType::NONE)
        {
            continue;
        }
        tiles[tilesToDraw[i]].Draw(window);
    }

    // shows tested area for collision resolution
        // auto cp = FindCollidableTilesCoords(player.position, player.size);
        // for(auto elem : cp)
        // {
        //     Tile t = tiles[elem.y * MAP_WIDTH + elem.x];
        //     t.color = sf::Color::Cyan;
        //     t.Draw(window);
        // }
    

    player.Draw(window);
}

void Map::HandleMouseInput(Vector2 mousePos, float deltaTime)
{
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        int purpose = 0;
        int itemID = player.GetItemInHand(purpose);
        Vector2 mouseCoords = CalculateMouseCoords(mousePos);
        auto breakableTiles = FindBreakableTilesCoords(player.position, player.size);
        
        if(PointBoxCollision(mousePos, tiles[breakableTiles[0].y * MAP_WIDTH + breakableTiles[0].x].position - Vector2{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f},
                        tiles[breakableTiles[breakableTiles.size() - 1].y * MAP_WIDTH + breakableTiles[breakableTiles.size() - 1].x].position
                        + Vector2{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f}))
        {
            const int index = mouseCoords.y * (int)MAP_WIDTH + mouseCoords.x;
            if(purpose == 1)
            {
                std::cout << "Attack\n";
            }
            else if(purpose == 2)
            {
                Tile &tile = tiles[index];
                if(tile.type != ItemType::BORDER || tile.type != ItemType::NONE)
                {
                    const toolProperties toolP = toolPropertiesTable[itemID - ((int)ItemType::COUNT - tilePropertiesTable.size() + 1)];
                    tile.durability -= (player.strength + toolP.damage) * deltaTime;
                    if(tile.durability <= 0.0f)
                    {
                        player.FindPlaceForItemInInventory(tile.type);
                        tile.setTileProperties(ItemType::NONE);
                    }
                }
            }
            else if(purpose == 3 && player.canPlaceBlock)
            {
                Tile &tile = tiles[index]; 
                auto playerbb = GetPlayerBoundingBox();
                if(!PointBoxCollision(mousePos / TILE_SIZE, playerbb.first / TILE_SIZE, playerbb.second / TILE_SIZE) && tile.type == ItemType::NONE)
                {
                    if(tiles[index + 1].canPlaceBlock || tiles[index - 1].canPlaceBlock 
                    || tiles[(mouseCoords.y - 1) * (int)MAP_WIDTH + mouseCoords.x].canPlaceBlock 
                    || tiles[(mouseCoords.y + 1) * (int)MAP_WIDTH + mouseCoords.x].canPlaceBlock || tiles[index].canPlaceBlock)
                    {
                        player.PlaceBlock();
                        tile.setTileProperties((ItemType)itemID);
                    }
                }
            }
        }
    }
}

void Map::HandleCollision(float deltaTime)
{
    Vector2 cp, cn;
    float ct = 0.0f;
    std::vector<std::pair<Vector2, float>> z;

// retrive collison tiles
    std::array<Vector2, 12> possibleCollisions = FindCollidableTilesCoords(player.position, player.size);
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
}

std::array<Vector2, 12> Map::FindCollidableTilesCoords(Vector2 position, Vector2 size) const
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

std::vector<Vector2> Map::FindBreakableTilesCoords(Vector2 position, Vector2 size) const
{
    std::vector<Vector2>  BreakableTiles;
    Vector2 topLeftCorner = Floor((position - size / 2.0f) / TILE_SIZE) - Vector2{1, 1};
    Vector2 botRightCorner = Ceil((position + size / 2.0f) / TILE_SIZE) + Vector2{1, 1};
    if(topLeftCorner.x < 0)
    {
        topLeftCorner.x = 0;
    }
    if(topLeftCorner.y < 0)
    {
        topLeftCorner.y = 0;
    }
    if(botRightCorner.x >= MAP_WIDTH)
    {
        botRightCorner.x = MAP_WIDTH - 1;
    }
    if(botRightCorner.y >= MAP_HEIGHT)
    {
        botRightCorner.y = MAP_HEIGHT - 1;
    }
    for(int y = topLeftCorner.y; y <= botRightCorner.y; y++)
    {
        for(int x = topLeftCorner.x; x <= botRightCorner.x; x++)
        {
            BreakableTiles.emplace_back(x, y);
        }
    }
    return BreakableTiles;
}

std::pair<Vector2, Vector2> Map::GetPlayerBoundingBox() const
{
    if(!player.canJump)
    {
        return std::make_pair(Floor((player.position - player.size / 2.0f) - Vector2{TILE_SIZE, TILE_SIZE}),
                                Ceil((player.position + player.size / 2.0f + Vector2{TILE_SIZE, TILE_SIZE})));
    }
    return std::make_pair(Floor((player.position - player.size / 2.0f) - Vector2{TILE_SIZE, TILE_SIZE}),
                                Ceil((player.position + player.size / 2.0f + Vector2{TILE_SIZE, 0})));
    
}

void Map::Save()
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
    for(int i = 0; i < tiles.size(); i++)
    {
        mapData << tiles[i].position.x << ';' << tiles[i].position.y << ',' << (int)tiles[i].type << '\n';
    }
    mapData.close();
}

void Map::Load()
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
        tiles[i] = decodeTileInfo(line);
        i++;
    }
}

void Map::Generate(const std::array<float, MAP_WIDTH> &seed)
{
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            tiles[y * MAP_WIDTH + x] = Tile(Vector2{x * TILE_SIZE, y * TILE_SIZE}, ItemType::NONE);
        }
    }
    //creates land
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        int y = (int)(seed[x] * MAP_HEIGHT);
        for(int i = MAP_HEIGHT - 1; i >= y; i--)
        {
            ItemType type = ItemType::GRASS;
            if((float)i > MAP_HEIGHT * 0.7f)
            {
                type = ItemType::STONE;
            }
            tiles[i * MAP_WIDTH + x].setTileProperties(type);
        }
    }
    // creates border
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        // Top border
        tiles[i].setTileProperties(ItemType::BORDER);

        // Bottom border
        tiles[(MAP_HEIGHT - 1) * MAP_WIDTH + i].setTileProperties(ItemType::BORDER);
    }

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        // Left border
        tiles[MAP_WIDTH * i].setTileProperties(ItemType::BORDER);

        // Right border
        tiles[(i + 1) * MAP_WIDTH - 1].setTileProperties(ItemType::BORDER);
    }
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
    std::string itemType = line.substr(commaPosition + 1);
    tile.setTileProperties(static_cast<ItemType>(std::stoi(itemType)));

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

Vector2 CalculateMouseCoords(Vector2 mousePos)
{
    // fix names in this function cuz it looks like shit right now
    Vector2 mouseCoords = mousePos / TILE_SIZE;
    int mousePosX = 0;
    int mousePosY = 0;
    if(mouseCoords.x - (int)mouseCoords.x >= 0.5f)
    {
        mousePosX = (int)mouseCoords.x + 1;
    }
    else
    {
        mousePosX = (int)mouseCoords.x;
    }
    if(mouseCoords.y - (int)mouseCoords.y >= 0.5f)
    {
        mousePosY = (int)mouseCoords.y + 1;
    }
    else
    {
        mousePosY = (int)mouseCoords.y;
    }
    return {mousePosX, mousePosY};
}