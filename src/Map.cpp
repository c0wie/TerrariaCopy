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
    tiles.resize(MAP_WIDTH * MAP_HEIGHT);
}

void Map::Draw(Vector2 playerPosition, sf::RenderWindow &window) 
{
    const std::vector<Vector2> tilesToDraw = GetVisibleTilesCoords(playerPosition);
    for(int i = tilesToDraw.size() - 1; i >= 0; i--)
    {
        SafeGetTile(tilesToDraw[i]).Draw(window);
    }
    
    /*if(gameState == GS_INVENTORY)
    {
        const Vector2 windowCenter = {window.getView().getCenter().x, window.getView().getCenter().y};
        const Vector2 lopLeftInventory = {windowCenter.x - SCREEN_WIDTH / 2.0f + 12.5f, windowCenter.y - SCREEN_HEIGHT / 2.0f + 12.5f};
        const Vector2 botRightInventory = { windowCenter.x - SCREEN_WIDTH / 2.0f + 225.f,
                                 windowCenter.y - SCREEN_HEIGHT / 2.0f + 152.5f};

        sf::RectangleShape rec(botRightInventory - lopLeftInventory);
        rec.setFillColor(sf::Color::Magenta);
        rec.setPosition(lopLeftInventory);
        window.draw(rec);
    }*/
    /*if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && player.GetItemInHand().IsTool())
    {
        Vector2 pos1 = mousePos;
        if( Length(player.position - mousePos) > 4 * TILE_SIZE )
        {
            const Vector2 direction = Normalize(mousePos - player.position);
            // Cap the position at 50.0f units away from the player
            pos1 = player.position + direction * 50.0f;
        }
        sf::VertexArray ray(sf::Lines, 2) ;
        ray[0].position = player.position;
        ray[0].color = sf::Color::Magenta;
        ray[1].position = pos1;
        ray[1].color = sf::Color::Magenta;
        window.draw(ray);
    }*/
}

void Map::Generate()
{
    constexpr int STONE_LEVEL = MAP_HEIGHT * 0.5f;
    constexpr float IRON_SPAWN_CHANCE = 0.2f;
    constexpr float SILVER_SPAWN_CHANCE = 0.2f;
    constexpr float GOLD_SPAWN_CHANCE = 0.12f;
    constexpr float COPPER_SPAWN_CHANCE = 0.3f;
    
    const std::array<float, MAP_WIDTH> seed = PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 0.8f), 5, 1.0f);
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
            tiles[i * MAP_WIDTH + x].SetProperties(type);
        }
    }
#pragma endregion

#pragma region apply textures
    for(int x = 0; x < MAP_WIDTH - 1; x++)
    {
        for(int y = 0; y < MAP_HEIGHT - 1; y++)
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
        int y = STONE_LEVEL + rand() % (((MAP_HEIGHT - 1) - STONE_LEVEL) + 1);
        PlaceOrePatch({x, y}, Tile::GOLD, GOLD_SPAWN_CHANCE);
// place iron
        y = STONE_LEVEL + rand() % (((MAP_HEIGHT - 1) - STONE_LEVEL) + 1);
        PlaceOrePatch({x, y}, Tile::IRON, IRON_SPAWN_CHANCE);
// place silver
        y = STONE_LEVEL + rand() % (((MAP_HEIGHT - 1) - STONE_LEVEL) + 1);
        PlaceOrePatch({x, y}, Tile::SILVER, SILVER_SPAWN_CHANCE);
// place copper
        y = STONE_LEVEL + rand() % (((MAP_HEIGHT - 1) - STONE_LEVEL) + 1);
        PlaceOrePatch({x, y}, Tile::COPPER, COPPER_SPAWN_CHANCE);
    }
#pragma endregion

#pragma region place borders
    // creates border
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        // Top border
        UnsafeGetTile({x, 0}).SetProperties(Tile::BORDER);

        // Bottom border
        UnsafeGetTile({x, MAP_HEIGHT - 1}).SetProperties(Tile::BORDER);
    }

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        // Left border
        UnsafeGetTile({0, y}).SetProperties(Tile::BORDER);

        // Right border
        UnsafeGetTile({MAP_WIDTH - 1, y}).SetProperties(Tile::BORDER);
    }
#pragma endregion
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
}

void Map::HandleCollisions(Player &player, float deltaTime)
{
    Vector2 collisionPoint, collisionNormal;
    float collisonTime = 0.0f;
    std::vector<int> realCollisions;

// retrive collison tiles
    const std::vector<Vector2> possibleCollisions = GetCollidableTilesCoords(player.position, player.size);
    realCollisions.reserve(possibleCollisions.size());
    for(int i = 0; i < possibleCollisions.size(); i++)
    { 
        const Tile &tile = UnsafeGetTile(possibleCollisions[i]);
        if(!tile.isCollidable())
        {
            continue;
        }
        if(RectDynamicRectCollision(player, tile, collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            realCollisions.emplace_back(i);
        }
    }
// resolve player collision
    for(int collisionIndex : realCollisions)
    {
        if(RectDynamicRectCollision(player, UnsafeGetTile(possibleCollisions[collisionIndex]), collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            player.velocity += collisionNormal * Abs(player.velocity) * (1.0f - collisonTime);
            if(collisionNormal == Vector2{0.0f, -1.0f})
            {
                player.canJump = true;
            }
        }
    }
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
        tile.subtype = Vector2(0, 0);
        tile.SetProperties(Tile::LOG);

    }

    Tile &treeCrown = UnsafeGetTile({rootCoords.x, rootCoords.y - treeHeight});
    treeCrown.subtype = Vector2(rand() % 3, 0);
    treeCrown.SetProperties(Tile::TREETOP);
    return true;
}

void Map::PlaceOrePatch(Vector2 tileCoords, short oreType, float spawnChance)
{
    for(int x = tileCoords.x ; x < tileCoords.x + 3; x++)
    {
        for(int y = tileCoords.y ; y < tileCoords.y + 3; y++)
        {
            Tile &tile = SafeGetTile({x, y});
            if(tile.type != Tile::STONE)
            {
                spawnChance += 0.15 * spawnChance;
                continue;
            }
            const float randomNumber = (float)rand() / RAND_MAX;
            if(randomNumber <= spawnChance)
            {
                tile.SetProperties(oreType);
                tile.UpdateTextureRect(CheckTileIntersection({tileCoords.x, tileCoords.y}));
            }
            else
            {
                spawnChance += 0.15 * spawnChance;
            }
        }
    }
}

Tile *Map::Raycast(Vector2 startPosition, Vector2 targetPosition)
{
    const Vector2 direction = Normalize(targetPosition - startPosition);
    
    const float stepSize = TILE_SIZE / 2.0f; 
    Vector2 currentPosition = startPosition;

    while (Length(currentPosition - startPosition) < 4 * TILE_SIZE) 
    {
        const Vector2 tileCoords = Round(currentPosition / TILE_SIZE); 

        Tile& tile = SafeGetTile(tileCoords);
        if (!tile.isNone())
        {
            return &tile; 
        }
        currentPosition += direction * stepSize;
    }
    return nullptr;  
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

std::vector<Vector2> Map::GetCollidableTilesCoords(Vector2 playerPosition, Vector2 playerSize) const
{
    return GetTileCoordsInArea(tiles, playerPosition, playerSize);
}

std::vector<Vector2> Map::GetBreakableTilesCoords(Vector2 playerPosition, Vector2 playerSize) const
{
    return GetTileCoordsInArea(tiles, playerPosition,  2 * playerSize);
}

std::vector<Vector2> Map::GetVisibleTilesCoords(Vector2 playerPosition) const
{
    return GetTileCoordsInArea(tiles, playerPosition, Vector2(SCREEN_WIDTH + 100.0f, SCREEN_HEIGHT + 100.0f));
}

std::vector<Vector2> Map::GetPlayerBBTilesCoords(Vector2 playerPosition) const
{
    return GetTileCoordsInArea(tiles, playerPosition, Vector2{TILE_SIZE, TILE_SIZE});
}

std::vector<Vector2> Map::GetTreeTilesCoords(Vector2 treeTileCoords) const
{
    std::vector<Vector2> treeTiles;
    treeTiles.reserve(16);

    int iterator = 0;
    while(true)
    {
        const Tile &tile = SafeGetTile({treeTileCoords.x, treeTileCoords.y - iterator});
        if( tile.type == Tile::LOG || tile.type == Tile::TREETOP)
        {
            treeTiles.push_back(tile.GetCoords());
            iterator++;
        }
        else 
        {
            iterator = 0;
            break;
        }
    }
    while(true)
    {
        const Tile &tile = SafeGetTile({treeTileCoords.x, treeTileCoords.y + iterator});
        if( tile.type == Tile::LOG)
        {
            treeTiles.push_back(tile.GetCoords());
            iterator++;
        }
        else 
        {
            break;
        }
    }
    return treeTiles;
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

bool Map::IsValidCoords(Vector2 coords) const
{
    if(coords.x < 0 || coords.x >= MAP_WIDTH)
    {
        return false;
    }
    if(coords.y < 0 || coords.y >= MAP_HEIGHT)
    {
        return false;
    }
    return true;
}

std::vector<Vector2> GetTileCoordsInArea(const std::vector<Tile> &map, Vector2 areaCenter, Vector2 areaSize)
{
    std::vector<Vector2> tilesCoordsInArea;
    const Vector2 halfAreaSize = areaSize / 2.0f;
    Vector2 topLeftTile = Floor((areaCenter - halfAreaSize) / TILE_SIZE);
    Vector2 botRightTile = Ceil((areaCenter + halfAreaSize) / TILE_SIZE);
   
    topLeftTile = {std::clamp((int)topLeftTile.x, 0, MAP_WIDTH - 1), std::clamp((int)topLeftTile.y, 0, MAP_HEIGHT - 1)};
    botRightTile = {std::clamp((int)botRightTile.x, 0, MAP_WIDTH - 1), std::clamp((int)botRightTile.y, 0, MAP_HEIGHT - 1)};

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