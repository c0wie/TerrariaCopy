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
            UnsafeGetTile({x, y}).Draw(window);
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

    if(playerItem.isWeapon())
    {
        std::cout << "Attack\n";
    }
    else if(playerItem.isTool())
    {
        Tile &tile = UnsafeGetTile({mouseCoords.x, mouseCoords.y});
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
        Tile &tile = UnsafeGetTile({mouseCoords.x, mouseCoords.y}); 
        const std::vector<Vector2> playerbb = GetPlayerBBTilesCoords();
        if(!PointBoxCollision(mouseCoords, playerbb[0],
                                playerbb[playerbb.size() - 1]) && tile.isNone())
        {
            if(!UnsafeGetTile({mouseCoords.x + 1, mouseCoords.y}).isNone()
                || !UnsafeGetTile({mouseCoords.x - 1, mouseCoords.y}).isNone() 
                || !UnsafeGetTile({mouseCoords.x, mouseCoords.y - 1}).isNone() 
                || !UnsafeGetTile({mouseCoords.x, mouseCoords.y + 1}).isNone())
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
        Tile &tile = UnsafeGetTile({possibleCollisions[i].x, possibleCollisions[i].y});
        if(!tile.isCollidable())
        {
            continue;
        }
        if(RectDynamicRectCollision(player, tile, collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            actualCollisions.emplace_back(possibleCollisions[i]);
        }
    }
// resolve player collision
    for(Vector2 collision : actualCollisions)
    {
        if(RectDynamicRectCollision(player, UnsafeGetTile({collision.x, collision.y}), collisionPoint, collisionNormal, collisonTime, deltaTime))
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

    Tile &downTile = UnsafeGetTile({x, y + 1});
    downTile.UpdateTextureRect(CheckTileIntersection( {x, y + 1} ));

    Tile &leftTile = UnsafeGetTile({x - 1, y});
    leftTile.UpdateTextureRect(CheckTileIntersection( {x - 1, y} ));
    
    Tile &topTile = UnsafeGetTile({x, y - 1});
    topTile.UpdateTextureRect(CheckTileIntersection( {x, y - 1} ));
    
    Tile &rightTile = UnsafeGetTile({x + 1, y});
    rightTile.UpdateTextureRect(CheckTileIntersection( {x + 1, y} ));
}

short Map::CheckTileIntersection(Vector2 coords)
{
    const int y = coords.y;
    const int x = coords.x;
    short intersectionFlag = 0b0000;

    if(SafeGetTile({x, y + 1}).isCollidable())
    {
        intersectionFlag ^= BOTTOM_INTERSECTION;
    }
    if(SafeGetTile({x - 1, y}).isCollidable())
    {
        intersectionFlag ^= LEFT_INTERSECTION;
    }
    if(SafeGetTile({x, y - 1}).isCollidable())
    {
        intersectionFlag ^= TOP_INTERSECTION;
    }
    if(SafeGetTile({x + 1, y}).isCollidable())
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

Tile &Map::SafeGetTile(Vector2 coords)
{
    static Tile stub = {}; 
    if( !IsValidCoords(coords) )
    {
        return stub;
    }
    return tiles[coords.y * MAP_WIDTH + coords.x];
}

const Tile& Map::SafeGetTile(Vector2 coords) const
{
    static Tile stub{};  
    
    if (!IsValidCoords(coords))
    {
        return stub;  
    }
    
    return tiles[coords.y * MAP_WIDTH + coords.x];  
}

Tile &Map::UnsafeGetTile(Vector2 coords)
{
    return tiles[coords.y * MAP_WIDTH + coords.x];
}

const Tile& Map::UnsafeGetTile(Vector2 coords) const
{
    return tiles[coords.y * MAP_WIDTH + coords.x];  
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
            UnsafeGetTile({x, y}) = Tile(Vector2{x * TILE_SIZE, y * TILE_SIZE}, Tile::Type::NONE);
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

#pragma region apply textures
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        for(int y = 0; y < MAP_HEIGHT; y++)
        {
            Tile &tile = UnsafeGetTile({x, y});
            if(!tile.HasTexture())
            {
                continue;
            }
            tile.UpdateTextureRect(CheckTileIntersection({x, y}));
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
        if( PlaceTree({x , y}) )
        {
            UnsafeGetTile({x, y}).UpdateTextureRect(CheckTileIntersection({x, y}));
        }
    }
#pragma endregion

#pragma region place ores

    for(int x = 2; x < MAP_WIDTH; x++)
    {
        if(x <= MAP_WIDTH - 3)
        {
            x += 2;
        }
        else 
        {
            break;
        }
// place gold
        int y = STONE_LEVEL + rand() % ((49 - STONE_LEVEL) + 1);
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if( PlaceOre({mX, mY}, Tile::GOLD, GOLD_SPAWN_CHANCE) )
                {
                    UnsafeGetTile({x, y}).UpdateTextureRect(CheckTileIntersection({x, y}));
                }
            }
        }
// place iron
        y = STONE_LEVEL + rand() % ((49 - STONE_LEVEL) + 1);
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if( PlaceOre({mX, mY}, Tile::IRON, IRON_SPAWN_CHANCE) )
                {
                    UnsafeGetTile({x, y}).UpdateTextureRect(CheckTileIntersection({x, y}));
                }
            }
        }
// place silver
        y = STONE_LEVEL + rand() % ((49 - STONE_LEVEL) + 1);
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if( PlaceOre({mX, mY}, Tile::SILVER, SILVER_SPAWN_CHANCE) )
                {
                    UnsafeGetTile({x, y}).UpdateTextureRect(CheckTileIntersection({x, y}));
                }
            }
        }
// place copper
        y = STONE_LEVEL + rand() % ((49 - STONE_LEVEL) + 1);
        for(int mX = x; mX < x + 2; mX++)
        {
            for(int mY = y; mY < y + 2; mY++)
            {
                if( PlaceOre({mX, mY}, Tile::COPPER, COPPER_SPAWN_CHANCE) )
                {
                    UnsafeGetTile({x, y}).UpdateTextureRect(CheckTileIntersection({x, y}));
                }
            }
        }
    }
#pragma endregion

#pragma region place borders
    // creates border
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        // Top border
        UnsafeGetTile({x, 0}).SetTileProperties(Tile::BORDER);

        // Bottom border
        UnsafeGetTile({x, MAP_HEIGHT - 1}).SetTileProperties(Tile::BORDER);
    }

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        // Left border
        UnsafeGetTile({0, y}).SetTileProperties(Tile::BORDER);

        // Right border
        UnsafeGetTile({MAP_WIDTH - 1, y}).SetTileProperties(Tile::BORDER);
    }
#pragma endregion
}

bool Map::PlaceTree(Vector2 rootCoords)
{
    const short rootType = UnsafeGetTile({rootCoords.x, rootCoords.y}).type;
    const Vector2 rootSubtype = UnsafeGetTile({rootCoords.x, rootCoords.y}).subtype;
    const bool placeTree = rand() % 3 <= 1 ;
    if(!placeTree)
    {
        return false;
    }
    // tree must be placed on grass
    if(rootType != Tile::GRASS)
    {
        return false;
    }
    if(rootSubtype == Vector2(1, 3) || rootSubtype == Vector2(2, 3))
    {
        return false;
    }
    // Tree height without tree crown
    const int treeHeight = rand() % 8 + 8;    
    // tree is too high
    if(rootCoords.y - treeHeight < 4)
    {
        return false;
    }

    for(int i = 0; i < treeHeight - 1; i++)
    {
        Tile &tile = UnsafeGetTile({rootCoords.x, rootCoords.y - ( i + 1)});
        tile.SetTileProperties(Tile::LOG);

        tile.subtype = Vector2(0, 0);
    }

    Tile &treeCrown = UnsafeGetTile({rootCoords.x, rootCoords.y - (treeHeight + 1)});
    treeCrown.SetTileProperties(Tile::TREETOP);
    treeCrown.subtype = Vector2(rand() % 3, 0);
    return true;
}

bool Map::PlaceOre(Vector2 tileCoords, short oreType, float spawnChance)
{
    Tile &tile = SafeGetTile({tileCoords.x, tileCoords.y});
    if(tile.type != Tile::STONE)
    {
       return false;
    }
    const float randomNumber = (float)rand() / RAND_MAX;
    if(randomNumber <= spawnChance)
    {
        tile.SetTileProperties(oreType);
        tile.UpdateTextureRect(CheckTileIntersection({tileCoords.x, tileCoords.y}));
    }
    return true;
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
    mouseCoords.x = mouseCoords.x - (int)mouseCoords.x >= 0.5f ? ceil(mouseCoords.x) : floor(mouseCoords.x);
    mouseCoords.y = mouseCoords.y - (int)mouseCoords.y >= 0.5f ? ceil(mouseCoords.y) : floor(mouseCoords.y);
    return mouseCoords;
}