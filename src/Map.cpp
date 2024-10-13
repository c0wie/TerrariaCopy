#include "Map.hpp"
#include "Collision.hpp"
#include "Math.hpp"
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;

Map::Map()
{
    if( !backgroundTxt.loadFromFile("resources/background.png") )
    {
        std::cout << "Unable to load file background.png\n";
    }
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
    sf::RectangleShape background({SCREEN_WIDTH + 70.0f, SCREEN_HEIGHT + 70.0f});
    background.setOrigin(background.getSize() / 2.0f);
    background.setTexture(&backgroundTxt);
    background.setPosition(player.position);
    window.draw(background);

    auto tilesToDraw = GetTilesToDraw(player.position);
    for(int i = 0; i < tilesToDraw.size(); i++)
    {
        const int index = tilesToDraw[i];
        if(tiles[index].isNone())
        {
            continue;
        }
        tiles[index].Draw(window);
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
    if(!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        return;
    }
    const Item playerItem = player.GetItemInHand();
    const Vector2 mouseCoords = CalculateMouseCoords(mousePos);
    const std::vector<Vector2> breakableTiles = GetBreakableTilesCoords(player.position, player.size);
    
    // Checks if user is clicking in player's range 
    if(!PointBoxCollision(mousePos, tiles[breakableTiles[0].y * MAP_WIDTH + breakableTiles[0].x].position - Vector2{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f},
                    tiles[breakableTiles[breakableTiles.size() - 1].y * MAP_WIDTH + breakableTiles[breakableTiles.size() - 1].x].position
                    + Vector2{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f}))
    {
        return;
    }

    const int index = mouseCoords.y * (int)MAP_WIDTH + mouseCoords.x;
    if(playerItem.isWeapon())
    {
        std::cout << "Attack\n";
    }
    else if(playerItem.isTool())
    {
        Tile &tile = tiles[index];
        if(!tile.isNone())
        {
            tile.durability -= (player.strength + playerItem.damage) * deltaTime;
            if(tile.durability <= 0.0f)
            {
                player.FindPlaceForItemInInventory(tile.type);
                tile.SetTileProperties(Tile::NONE);
                UpdateSurroundingTiles({mouseCoords.x, mouseCoords.y});
            }
        }
    }
    else if(playerItem.isBlock() && player.canPlaceBlock)
    {
        Tile &tile = tiles[index]; 
        auto playerbb = GetPlayerBoundingBox();
        if(!PointBoxCollision(mousePos / TILE_SIZE, playerbb.first / TILE_SIZE, playerbb.second / TILE_SIZE) && tile.isNone())
        {
            if(!tiles[index + 1].isNone() || !tiles[index - 1].isNone() 
            || !tiles[(mouseCoords.y - 1) * (int)MAP_WIDTH + mouseCoords.x].isNone() 
            || !tiles[(mouseCoords.y + 1) * (int)MAP_WIDTH + mouseCoords.x].isNone())
            {
                player.PlaceBlock();
                tile.SetTileProperties(playerItem.type);
                tile.UpdateTextureRect(CheckTileIntersection({mouseCoords.x, mouseCoords.y}));
                UpdateSurroundingTiles({mouseCoords.x, mouseCoords.y});
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
    std::array<Vector2, 20> possibleCollisions = GetCollidableTilesCoords(player.position, player.size);
    for(int i = 0; i < possibleCollisions.size(); i++)
    { 
        const int x = possibleCollisions[i].x;
        const int y = possibleCollisions[i].y;
        if(!tiles[y * MAP_WIDTH + x].isCollidable())
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

std::array<Vector2, 20> Map::GetCollidableTilesCoords(Vector2 position, Vector2 size) const
{
    int i = 0;
    std::array<Vector2, 20>  CollideTiles;
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

std::vector<Vector2> Map::GetBreakableTilesCoords(Vector2 position, Vector2 size) const
{
    std::vector<Vector2>  BreakableTiles;
    Vector2 topLeftCorner = Floor((position - size / 2.0f) / TILE_SIZE) - Vector2{1, 1};
    Vector2 botRightCorner = Ceil((position + size / 2.0f) / TILE_SIZE) + Vector2{1, 1};

    topLeftCorner = {std::clamp((int)topLeftCorner.x, 0, MAP_WIDTH), std::clamp((int)topLeftCorner.y, 0, MAP_HEIGHT)};
    botRightCorner = {std::clamp((int)botRightCorner.x, 0, MAP_WIDTH), std::clamp((int)botRightCorner.y, 0, MAP_HEIGHT)};

    for(int y = topLeftCorner.y; y <= botRightCorner.y; y++)
    {
        for(int x = topLeftCorner.x; x <= botRightCorner.x; x++)
        {
            BreakableTiles.emplace_back(x, y);
        }
    }
    return BreakableTiles;
}

std::vector<int> Map::GetTilesToDraw(Vector2 playerPosition) const
{
    std::vector<int> tilesToDraw;
    constexpr Vector2 halfScreenSize = Vector2{SCREEN_WIDTH / 2.0f + 100.0f, SCREEN_HEIGHT / 2.0f + 100.0f};
    Vector2 topLeftCorner = Vector2{playerPosition.x - halfScreenSize.x, playerPosition.y + halfScreenSize.y};
    Vector2 botRightCorner = Vector2{playerPosition.x + halfScreenSize.x, playerPosition.y - halfScreenSize.y};
    Vector2 topLeftTile = Ceil(topLeftCorner / TILE_SIZE);
    Vector2 botRightTile = Ceil(botRightCorner / TILE_SIZE);
   
    topLeftTile = {std::clamp((int)topLeftTile.x, 0, MAP_WIDTH), std::clamp((int)topLeftTile.y, 0, MAP_HEIGHT)};
    botRightTile = {std::clamp((int)botRightTile.x, 0, MAP_WIDTH), std::clamp((int)botRightTile.y, 0, MAP_HEIGHT)};
    for(int x = topLeftTile.x; x < botRightTile.x; x++)
    {
        for(int y = botRightTile.y; y < topLeftTile.y; y++)
        {
            tilesToDraw.push_back(y * MAP_WIDTH + x);
        }
    }
    return tilesToDraw;
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

void Map::UpdateSurroundingTiles(Vector2 centerTileindex)
{
    const int y = centerTileindex.y;
    const int x = centerTileindex.x;

    if(y != (MAP_HEIGHT - 1))
    {
        Tile &tile = tiles[(y + 1) * MAP_WIDTH + x];
        tile.UpdateTextureRect(CheckTileIntersection({x, (y + 1)}));
    }
    if(x != 0)
    {
        Tile &tile = tiles[y * MAP_WIDTH + (x - 1)];
        tile.UpdateTextureRect(CheckTileIntersection({(x - 1), y}));
    }
    if(y != 0)
    {
        Tile &tile = tiles[(y - 1) * MAP_WIDTH + x];
        tile.UpdateTextureRect(CheckTileIntersection({x, (y - 1)}));
    }
    if(x != (MAP_WIDTH - 1))
    {
        Tile &tile = tiles[y * MAP_WIDTH + (x + 1)];
        tile.UpdateTextureRect(CheckTileIntersection({(x + 1), y}));
    }
}

short Map::CheckTileIntersection(Vector2 index)
{
    const int y = index.y;
    const int x = index.x;
    short intersectionFlag = 0b0000;
    if(y != (MAP_HEIGHT - 1) && tiles[(y + 1) * (int)MAP_WIDTH + x].type != Tile::NONE)
    {
        intersectionFlag ^= BOTTOM_INTERSECTION;
    }
    if(x != 0 && tiles[y * (int)MAP_WIDTH + (x - 1)].type != Tile::NONE)
    {
        intersectionFlag ^= LEFT_INTERSECTION;
    }
    if(y != 0 && tiles[(y - 1) * (int)MAP_WIDTH + x].type != Tile::NONE)
    {
        intersectionFlag ^= TOP_INTERSECTION;
    }
    if(x != ((int)MAP_WIDTH - 1) && tiles[y * (int)MAP_WIDTH + (x + 1)].type != Tile::NONE)
    {
        intersectionFlag ^= RIGHT_INTERSECTION;
    }
    return intersectionFlag;
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
        mapData << tiles[i].GetInfo() << '\n';
    }
    mapData.close();
    player.Save();
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
        tiles[i].Load(line);
        i++;
    }
    player.Load();
}

void Map::Generate()
{
    const std::array<float, MAP_WIDTH> seed = PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 1.0f), 5, 0.75f);
    
#pragma region generate terrain
    // assigning postions for all tiles
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            tiles[y * MAP_WIDTH + x] = Tile(Vector2{x * TILE_SIZE, y * TILE_SIZE}, Tile::Type::NONE);
        }
    }
    //creates land
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        int y = (int)(seed[x] * MAP_HEIGHT);
        for(int i = MAP_HEIGHT - 1; i >= y; i--)
        {
            short type = Tile::Type::GRASS;
            if((float)i > MAP_HEIGHT * 0.7f)
            {
                type = Tile::Type::STONE;
            }
            tiles[i * MAP_WIDTH + x].SetTileProperties(type);
        }
    }
#pragma endregion

#pragma region place trees

    for(int x = 2; x < MAP_WIDTH; x++)
    {
        const int y = (int)(seed[x] * MAP_HEIGHT);
        const bool placeTree = rand() % 12 < 4 ;
        if(!placeTree)
        {
            continue;
        }
        if(x <= MAP_WIDTH - 3)
        {
            x += 2;
        }
        else 
        {
            break;
        }
        PlaceTree({x , y}, tiles[y * MAP_WIDTH + x].type);
    }

#pragma endregion

#pragma region place borders
    // creates border
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        // Top border
        tiles[i].SetTileProperties(Tile::Type::BORDER);

        // Bottom border
        tiles[(MAP_HEIGHT - 1) * MAP_WIDTH + i].SetTileProperties(Tile::Type::BORDER);
    }

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        // Left border
        tiles[MAP_WIDTH * i].SetTileProperties(Tile::Type::BORDER);

        // Right border
        tiles[(i + 1) * MAP_WIDTH - 1].SetTileProperties(Tile::Type::BORDER);
    }
#pragma endregion

#pragma region apply textures
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            Tile &tile = tiles[y * MAP_WIDTH + x];
            if(tile.type == Tile::NONE || tile.type == Tile::BORDER)
            {
                continue;
            }
            tile.UpdateTextureRect(CheckTileIntersection({x, y}));
        }
    }
#pragma endregion
}

void Map::PlaceTree(Vector2 rootCoords, short rootType)
{
    // tree must be placed on grass
    if(rootType != Tile::GRASS)
    {
        return;
    }
    // Tree height without tree crown
    const int treeHeight = rand() % 8 + 8;    
    // tree is too high
    if(rootCoords.y - treeHeight < 3)
    {
        return;
    }

    for(int i = 0; i < treeHeight - 1; i++)
    {
        Tile &tile = tiles[(rootCoords.y - (i + 1)) * MAP_WIDTH + rootCoords.x];
        if(tile.type != Tile::NONE)
        {
            continue;
        }
        tile.SetTileProperties(Tile::LOG);
        tile.subtype = Vector2(0, 0);
    }

    tiles[(rootCoords.y - (treeHeight + 1)) * MAP_WIDTH + rootCoords.x].SetTileProperties(Tile::TREETOP);
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
    return {(float)mousePosX, (float)mousePosY};
}