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
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        HandleMouseInput(mousePos, deltaTime);
    }
    player.Update(deltaTime);
    HandleCollisions(deltaTime);
    player.Move(player.velocity * deltaTime);
}

void Map::Draw(sf::RenderWindow &window) const
{
    sf::RectangleShape background({SCREEN_WIDTH + TILE_SIZE * 3, SCREEN_HEIGHT + TILE_SIZE * 3});
    background.setOrigin(background.getSize() / 2.0f);
    background.setTexture(&backgroundTxt);
    background.setPosition(player.position);
    window.draw(background);

    std::vector<Vector2> tilesToDraw = GetVisibleTilesCoords();
    for(int x = tilesToDraw[0].x; x < tilesToDraw[tilesToDraw.size() - 1].x; x++)
    {
        for(int y = tilesToDraw[0].y; y < tilesToDraw[tilesToDraw.size() - 1].y; y++)
        {
            tiles[y * MAP_WIDTH + x].Draw(window);
        }
    }
    player.Draw(window);
}

void Map::HandleMouseInput(Vector2 mousePos, float deltaTime)
{
    const Item playerItem = player.GetItemInHand();
    const Vector2 mouseCoords = CalculateMouseCoords(mousePos);
    const std::vector<Vector2> breakableTiles = GetBreakableTilesCoords();
    
    // Checks if user is clicking in player's range 
    if(!PointBoxCollision(mouseCoords, breakableTiles[0], breakableTiles[breakableTiles.size() - 1]))
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
        const std::vector<Vector2> playerbb = GetPlayerBBTilesCoords();
        if(!PointBoxCollision(mouseCoords, playerbb[0],
                                playerbb[playerbb.size() - 1]) && tile.isNone())
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

void Map::HandleCollisions(float deltaTime)
{
    Vector2 collisionPoint, collisionNormal;
    float collisonTime = 0.0f;
    std::vector<Vector2> actualCollisions;

// retrive collison tiles
    const std::vector<Vector2> possibleCollisions = GetCollidableTilesCoords();
    actualCollisions.reserve(possibleCollisions.size());
    for(int i = 0; i < possibleCollisions.size(); i++)
    { 
        const int x = possibleCollisions[i].x;
        const int y = possibleCollisions[i].y;
        if(!tiles[y * MAP_WIDTH + x].isCollidable())
        {
            continue;
        }
        if(RectDynamicRectCollision(player, tiles[y * MAP_WIDTH + x], collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            actualCollisions.emplace_back(possibleCollisions[i]);
        }
    }
// resolve player collision
    for(Vector2 collision : actualCollisions)
    {
        if(RectDynamicRectCollision(player, tiles[collision.y * MAP_WIDTH + collision.x], collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            player.velocity += collisionNormal * Abs(player.velocity) * (1.0f - collisonTime);
            if(collisionNormal == Vector2{0.0f, -1.0f})
            {
                player.canJump = true;
            }
        }
    }
}

std::vector<Vector2> Map::GetCollidableTilesCoords() const
{
    return GetTileCoordsInArea(tiles, player.position, player.size);
}

std::vector<Vector2> Map::GetBreakableTilesCoords() const
{
    return GetTileCoordsInArea(tiles, player.position,  2 * player.size);
}

std::vector<Vector2> Map::GetVisibleTilesCoords() const
{
    return GetTileCoordsInArea(tiles, player.position, Vector2(SCREEN_WIDTH + 100.0f, SCREEN_HEIGHT + 100.0f));
}

std::vector<Vector2> Map::GetPlayerBBTilesCoords() const
{
    return GetTileCoordsInArea(tiles, player.position, Vector2{TILE_SIZE, TILE_SIZE});
}

void Map::UpdateSurroundingTiles(Vector2 centerTileCoords)
{
    const float y = centerTileCoords.y;
    const float x = centerTileCoords.x;

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

short Map::CheckTileIntersection(Vector2 coords)
{
    const int y = coords.y;
    const int x = coords.x;
    short intersectionFlag = 0b0000;
    if(y != (MAP_HEIGHT - 1) && tiles[(y + 1) * (int)MAP_WIDTH + x].isCollidable())
    {
        intersectionFlag ^= BOTTOM_INTERSECTION;
    }
    if(x != 0 && tiles[y * (int)MAP_WIDTH + (x - 1)].isCollidable())
    {
        intersectionFlag ^= LEFT_INTERSECTION;
    }
    if(y != 0 && tiles[(y - 1) * (int)MAP_WIDTH + x].isCollidable())
    {
        intersectionFlag ^= TOP_INTERSECTION;
    }
    if(x != ((int)MAP_WIDTH - 1) && tiles[y * (int)MAP_WIDTH + (x + 1)].isCollidable())
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

bool Map::IsValidCoords(Vector2 coords) const
{
    if(coords.x < 0 || coords.x >= MAP_WIDTH)
    {
        return false;
    }
    if(coords.y < 0 || coords.y > MAP_HEIGHT)
    {
        return false;
    }
    return true;
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
    constexpr int STONE_LEVEL = MAP_HEIGHT * 0.7f;
    constexpr float IRON_SPAWN_CHANCE = 0.2f;
    constexpr float SILVER_SPAWN_CHANCE = 0.2f;
    constexpr float GOLD_SPAWN_CHANCE = 0.12f;
    constexpr float COPPER_SPAWN_CHANCE = 0.3f;

    const std::array<float, MAP_WIDTH> seed = PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 1.0f), 5, 1.0f);
#pragma region generate terrain
    // assigning postions for all tiles
    for(int x = MAP_WIDTH - 1; x >= 0; x--)
    {
        for(int y = MAP_HEIGHT - 1; y >= 0; y--)
        {
            tiles[y * MAP_WIDTH + x] = Tile(Vector2{x * TILE_SIZE, y * TILE_SIZE}, Tile::Type::NONE);
        }
    }
    //creates land
    for(int x = MAP_WIDTH - 1; x >= 0; x--)
    {
        const int y = (int)(seed[x] * MAP_HEIGHT);
        for(int i = MAP_HEIGHT - 1; i >= y; i--)
        {
            short type = Tile::GRASS;
            if(i > STONE_LEVEL)
            {
                type = Tile::STONE;
            }
            tiles[i * MAP_WIDTH + x].SetTileProperties(type);
        }
    }
#pragma endregion

#pragma region place trees

    for(int x = 2; x < MAP_WIDTH; x++)
    {
        const int y = (int)(seed[x] * MAP_HEIGHT);
        if(x <= MAP_WIDTH - 3)
        {
            x += 2;
        }
        else 
        {
            break;
        }
        PlaceTree({x , y}, tiles[y * MAP_WIDTH + x].type, tiles[y * MAP_WIDTH + x].subtype);
    }

#pragma endregion

#pragma region place ores

    for(int x = 2; x < MAP_WIDTH; x++)
    {
        const int y = STONE_LEVEL + rand() % ((49 - STONE_LEVEL) + 1);
        if(x <= MAP_WIDTH - 3)
        {
            x += 2;
        }
        else 
        {
            break;
        }
// place gold
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if(!IsValidCoords({mX, mY}))
                {
                    break;
                }
                Tile &tile = tiles[mY * MAP_WIDTH + mX]; 
                if(tile.type != Tile::STONE)
                {
                   break;
                }
                const float randomNumber = (float)rand() / RAND_MAX;
                if(randomNumber <= GOLD_SPAWN_CHANCE)
                {
                    tile.SetTileProperties(Tile::GOLD);
                    tile.UpdateTextureRect(CheckTileIntersection({mX, mY}));
                }
            }
        }
// place iron
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if(!IsValidCoords({mX, mY}))
                {
                    break;
                }
                Tile &tile = tiles[mY * MAP_WIDTH + mX]; 
                if(tile.type != Tile::STONE)
                {
                   break;
                }
                const float randomNumber = (float)rand() / RAND_MAX;
                if(randomNumber <= IRON_SPAWN_CHANCE)
                {
                    tile.SetTileProperties(Tile::IRON);
                    tile.UpdateTextureRect(CheckTileIntersection({mX, mY}));
                }
            }
        }
// place silver
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if(!IsValidCoords({mX, mY}))
                {
                    break;
                }
                Tile &tile = tiles[mY * MAP_WIDTH + mX]; 
                if(tile.type != Tile::STONE)
                {
                   break;
                }
                const float randomNumber = (float)rand() / RAND_MAX;
                if(randomNumber <= SILVER_SPAWN_CHANCE)
                {
                    tile.SetTileProperties(Tile::SILVER);
                    tile.UpdateTextureRect(CheckTileIntersection({mX, mY}));
                }
            }
        }
// place copper
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if(!IsValidCoords({mX, mY}))
                {
                    break;
                }
                Tile &tile = tiles[mY * MAP_WIDTH + mX]; 
                if(tile.type != Tile::STONE)
                {
                   break;
                }
                const float randomNumber = (float)rand() / RAND_MAX;
                if(randomNumber <= COPPER_SPAWN_CHANCE)
                {
                    tile.SetTileProperties(Tile::COPPER);
                    tile.UpdateTextureRect(CheckTileIntersection({mX, mY}));
                }
            }
        }
    }
#pragma endregion

#pragma region place borders
    // creates border
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        // Top border
        tiles[i].SetTileProperties(Tile::BORDER);

        // Bottom border
        tiles[(MAP_HEIGHT - 1) * MAP_WIDTH + i].SetTileProperties(Tile::BORDER);
    }

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        // Left border
        tiles[MAP_WIDTH * i].SetTileProperties(Tile::BORDER);

        // Right border
        tiles[(i + 1) * MAP_WIDTH - 1].SetTileProperties(Tile::BORDER);
    }
#pragma endregion

#pragma region apply textures
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            Tile &tile = tiles[y * MAP_WIDTH + x];
            if(!tile.HasTexture())
            {
                continue;
            }
            tile.UpdateTextureRect(CheckTileIntersection({x, y}));
        }
    }
#pragma endregion
}

void Map::PlaceTree(Vector2 rootCoords, short rootType, Vector2 subtype)
{
    const bool placeTree = rand() % 3 <= 1 ;
    if(!placeTree)
    {
        return;
    }
    // tree must be placed on grass
    if(rootType != Tile::GRASS)
    {
        return;
    }
    if(subtype == Vector2(1, 3) || subtype == Vector2(2, 3))
    {
        return;
    }
    // Tree height without tree crown
    const int treeHeight = rand() % 8 + 8;    
    // tree is too high
    if(rootCoords.y - treeHeight < 4)
    {
        return;
    }

    for(int i = 0; i < treeHeight - 1; i++)
    {
        Tile &tile = tiles[(rootCoords.y - (i + 1)) * MAP_WIDTH + rootCoords.x];
        tile.SetTileProperties(Tile::LOG);
        tile.subtype = Vector2(0, 0);
    }

    tiles[(rootCoords.y - (treeHeight + 1)) * MAP_WIDTH + rootCoords.x].SetTileProperties(Tile::TREETOP);
    tiles[(rootCoords.y - (treeHeight + 1)) * MAP_WIDTH + rootCoords.x].subtype = Vector2(rand() % 3, 0);
}

std::vector<Vector2>GetTileCoordsInArea(const std::array<Tile, MAP_WIDTH * MAP_HEIGHT> &map, Vector2 areaCenter, Vector2 areaSize)
{
    std::vector<Vector2> tilesCoordsInArea;
    const Vector2 halfAreaSize = areaSize / 2.0f;
    Vector2 topLeftTile = Floor((areaCenter - halfAreaSize) / TILE_SIZE);
    Vector2 botRightTile = Ceil((areaCenter + halfAreaSize) / TILE_SIZE);
   
    topLeftTile = {std::clamp((int)topLeftTile.x, 0, MAP_WIDTH), std::clamp((int)topLeftTile.y, 0, MAP_HEIGHT)};
    botRightTile = {std::clamp((int)botRightTile.x, 0, MAP_WIDTH), std::clamp((int)botRightTile.y, 0, MAP_HEIGHT)};

    const int tilesWidth = (int)botRightTile.x - (int)topLeftTile.x + 1;
    const int tilesHeight = (int)botRightTile.y - (int)topLeftTile.y + 1;

    tilesCoordsInArea.reserve(tilesWidth * tilesHeight);

    for(int x = topLeftTile.x; x <= botRightTile.x; x++)
    {
        for(int y = topLeftTile.y; y <= botRightTile.y; y++)
        {
            tilesCoordsInArea.emplace_back(x, y);
        }
    }
    return tilesCoordsInArea;
}

Vector2 CalculateMouseCoords(Vector2 mousePos)
{
    // fix names in this function cuz it looks like shit right now
    Vector2 mouseCoords = mousePos / TILE_SIZE;
    mouseCoords.x = mouseCoords.x - (int)mouseCoords.x >= 0.5f? ceil(mouseCoords.x) : floor(mouseCoords.x);
    mouseCoords.y = mouseCoords.y - (int)mouseCoords.y >= 0.5f? ceil(mouseCoords.y) : floor(mouseCoords.y);
    return mouseCoords;
}