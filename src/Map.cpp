#include "Map.hpp"
#include "PerlinNoise.hpp"
#include "Collision.hpp"
#include "Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <queue>

namespace fs = std::filesystem;

Map::Map()
{
    tiles.resize(MAP_WIDTH * MAP_HEIGHT);
    font.loadFromFile("resources/font.ttf");
    lightSources.emplace_back(MAP_WIDTH / 2, 1);
}

void Map::Draw(Vector2 playerPosition, sf::RenderWindow &window) 
{
    const std::vector<Vector2> tilesToDraw = GetVisibleTilesCoords(playerPosition);
    for(int i = tilesToDraw.size() - 1; i >= 0; i--)
    {
        SafeGetTile(tilesToDraw[i]).Draw(font, window);
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
    constexpr int SPAWN_ORES_LAYER = MAP_HEIGHT * 0.2f;
    constexpr float IRON_SPAWN_CHANCE = 0.2f;
    constexpr float SILVER_SPAWN_CHANCE = 0.2f;
    constexpr float GOLD_SPAWN_CHANCE = 0.12f;
    constexpr float COPPER_SPAWN_CHANCE = 0.3f;
    constexpr Vector2 GOLD_SPAWN_RANGE = Vector2(MAP_HEIGHT * 0.4f, MAP_HEIGHT);
    constexpr Vector2 IRON_SPAWN_RANGE = Vector2(MAP_HEIGHT * 0.67f, MAP_HEIGHT * 0.334f);
    constexpr Vector2 SILVER_SPAWN_RANGE = Vector2(MAP_HEIGHT * 0.56f, MAP_HEIGHT * 0.4f);
    constexpr Vector2 COPPER_SPAWN_RANGE = Vector2(MAP_HEIGHT * 0.8f, MAP_HEIGHT);
    
    const std::array<float, MAP_WIDTH> seed = PerlinNoise1D<MAP_WIDTH>(GenerateRandomArray<MAP_WIDTH>(0.0, 0.8f), 5, 2.0f);

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
        for(int i = MAP_HEIGHT - 1; i > y + 5; i--)
        {
            tiles[i * MAP_WIDTH + x].SetProperties(Tile::STONE);
        }
        for(int i = y + 5; i >= y; i--)
        {
            tiles[i * MAP_WIDTH + x].SetProperties(Tile::GRASS);
        }
    }
#pragma endregion

#pragma region generate caves
    const std::vector<float> spaghettiCave = PerlinNoise2D<MAP_WIDTH, MAP_HEIGHT>(GenerateRandomVector<MAP_WIDTH * MAP_HEIGHT>(0.0f, 1.0f), 5, 1);
    auto sortedSpaghettiCave = spaghettiCave;
    std::sort(sortedSpaghettiCave.begin(), sortedSpaghettiCave.end());

    const std::vector<float> cheeseCave = PerlinNoise2D<MAP_WIDTH, MAP_HEIGHT>(GenerateRandomVector<MAP_WIDTH * MAP_HEIGHT>(0.0f, 1.0f), 5, 1);
    auto sortedCheeseCave = cheeseCave;
    std::sort(sortedCheeseCave.begin(), sortedCheeseCave.end());

    for(int x = MAP_WIDTH - 10; x >= 0; x--)
    {
        const int y = (int)(seed[x] * MAP_HEIGHT);
        for(int i = MAP_HEIGHT - 10; i >= y; i--)
        {
            if(tiles[i * MAP_WIDTH + x].type == Tile::STONE || tiles[i * MAP_WIDTH + x].type == Tile::GRASS)
            {
                if(spaghettiCave[i * MAP_WIDTH + x] >= sortedSpaghettiCave[0.43f * sortedSpaghettiCave.size()] 
                && spaghettiCave[i * MAP_WIDTH + x] <= sortedSpaghettiCave[0.57f * sortedSpaghettiCave.size()])
                {
                    tiles[i * MAP_WIDTH + x].SetProperties(Tile::NONE);
                }
                else if(cheeseCave[i * MAP_WIDTH + x] > sortedCheeseCave[0.9f * sortedCheeseCave.size()])
                {
                    tiles[i * MAP_WIDTH + x].SetProperties(Tile::NONE);
                }
            }
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
// this scope just for this x variable to not get any naming conflicts
{
    int x = 2;
    while(x < MAP_WIDTH - 3)
    {
        if(rand() % 3 > 1)
        {
            x++;
            continue;
        }
        const int y = floor(seed[x] * MAP_HEIGHT);
        Tile &rootTile = UnsafeGetTile({x, y});
        if(rootTile.type != Tile::GRASS || rootTile.subtype == Vector2(1, 3) || rootTile.subtype == Vector2(2, 3))
        {
            x++;
            continue;
        }
        const int treeHeight = rand() % 8 + 8;    
        for(int i = 0; i < treeHeight - 1; i++)
        {
            Tile &tile = UnsafeGetTile({x, y - ( i + 1)});
            tile.subtype = Vector2(0, 0);
            tile.SetProperties(Tile::LOG);
        }
        Tile &treeCrown = UnsafeGetTile({x, y - treeHeight});
        treeCrown.subtype = Vector2(rand() % 3, 0);
        treeCrown.SetProperties(Tile::TREETOP);
        rootTile.UpdateTextureRect(CheckTileIntersection({x, y}));
        x += 2;
    }
    
}
#pragma endregion

#pragma region place ores
{
    int x = 2;
    int succesfulPositions = 0;
    while(x < MAP_WIDTH - 3)
    {
        const int y =  SPAWN_ORES_LAYER  + rand() % (((MAP_HEIGHT - 1) - SPAWN_ORES_LAYER) + 1);
        PlaceOrePatch({x, y}, Tile::COPPER, COPPER_SPAWN_CHANCE, succesfulPositions);
        x += succesfulPositions + 1;
    }
    x = 2;
    while(x < MAP_WIDTH - 3)
    {
        const int y =  SPAWN_ORES_LAYER + rand() % (((MAP_HEIGHT - 1) - SPAWN_ORES_LAYER) + 1);
        PlaceOrePatch({x, y}, Tile::SILVER, SILVER_SPAWN_CHANCE, succesfulPositions);
        x += succesfulPositions + 3;
    }
    x = 2;
    while(x < MAP_WIDTH - 3)
    {
        const int y =  SPAWN_ORES_LAYER + rand() % (((MAP_HEIGHT - 1) - SPAWN_ORES_LAYER) + 1);
        PlaceOrePatch({x, y}, Tile::IRON, IRON_SPAWN_CHANCE, succesfulPositions);
        x += succesfulPositions + 3;
    }
    x = 2;
    while(x < MAP_WIDTH - 3)
    {
        const int y =  SPAWN_ORES_LAYER + rand() % (((MAP_HEIGHT - 1) - SPAWN_ORES_LAYER) + 1);
        PlaceOrePatch({x, y}, Tile::GOLD, GOLD_SPAWN_CHANCE, succesfulPositions);
        x += succesfulPositions + 5;
    }
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
    UpdateLighting();
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
    for(int i = 0; i < tiles.size(); i++)
    {
        if(tiles[i].IsLightSource() )
        {
            lightSources.emplace_back(tiles[i].GetCoords() );
        }
    }
    UpdateLighting();
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

    Tile &topTile = UnsafeGetTile({x, y - 1});
    topTile.UpdateTextureRect(CheckTileIntersection( {x, y - 1} ));

    Tile &leftTile = UnsafeGetTile({x - 1, y});
    leftTile.UpdateTextureRect(CheckTileIntersection( {x - 1, y} ));
    
    Tile &downTile = UnsafeGetTile({x, y + 1});
    downTile.UpdateTextureRect(CheckTileIntersection( {x, y + 1} ));
    
    Tile &rightTile = UnsafeGetTile({x + 1, y});
    rightTile.UpdateTextureRect(CheckTileIntersection( {x + 1, y} ));
}

void Map::UpdateLighting()
{
    for(int x = MAP_WIDTH - 1; x >= 0; x--)
    {
        for(int y = MAP_HEIGHT - 1; y >= 0; y--)
        {
            UnsafeGetTile({x, y}).SetLighting(0);
        }
    }
    
    std::queue<Vector2> lightQueue;
     // lighting for sun which light don't weakens while it travel
    UnsafeGetTile(lightSources[0]).SetLighting(16);
    lightQueue.push(lightSources[0]);
    while (!lightQueue.empty())
    {
        Tile &tile = UnsafeGetTile({lightQueue.front().x, lightQueue.front().y});
        lightQueue.pop();
        const Vector2 coords = tile.GetCoords();
        if(tile.lightLevel == 0)
        {
            continue;
        }
        //top tile
        if( IsValidCoords({coords.x, coords.y - 1}) )
        {
            Tile &topTile = UnsafeGetTile({coords.x, coords.y - 1});
            if(topTile.lightLevel < tile.lightLevel)
            {
                topTile.SetLighting(tile.lightLevel);
                lightQueue.push(topTile.GetCoords());
            }
        }
        //left tile
        if( IsValidCoords({coords.x - 1, coords.y}))
        {
            Tile &leftTile = UnsafeGetTile({coords.x - 1, coords.y});
            if(leftTile.lightLevel < tile.lightLevel)
            {
                leftTile.SetLighting(tile.lightLevel);
                lightQueue.push(leftTile.GetCoords());
            }
        }
        //bottom tile
        if( IsValidCoords({coords.x, coords.y + 1}))
        {
            Tile &bottomTile = UnsafeGetTile({coords.x, coords.y + 1});
            if(bottomTile.lightLevel < tile.lightLevel)
            {
                bottomTile.SetLighting(tile.lightLevel);
                lightQueue.push(bottomTile.GetCoords());
            }
        }
        //right tile
        if( IsValidCoords({coords.x + 1, coords.y}))
        {
            Tile &rightTile = UnsafeGetTile({coords.x + 1, coords.y});
            if(rightTile.lightLevel < tile.lightLevel)
            {
                rightTile.SetLighting(tile.lightLevel);
                lightQueue.push(rightTile.GetCoords());
            }
        }
    }
    
    // lighting for rest of light sources
    for(int i = 1; i < lightSources.size(); i++)
    {
        UnsafeGetTile(lightSources[i]).SetLighting(16);
        lightQueue.push(lightSources[i]);
    }
    while (!lightQueue.empty())
    {
        Tile &tile = UnsafeGetTile({lightQueue.front().x, lightQueue.front().y});
        lightQueue.pop();
        const Vector2 coords = tile.GetCoords();
        if(tile.lightLevel == 0)
        {
            continue;
        }
        //top tile
        if( IsValidCoords({coords.x, coords.y - 1}) )
        {
            Tile &topTile = UnsafeGetTile({coords.x, coords.y - 1});
            if(topTile.lightLevel < tile.lightLevel)
            {
                if(topTile.lightConsumption == 0)
                {
                    topTile.SetLighting(tile.lightLevel - 2);
                }
                else
                {
                    topTile.SetLighting(tile.lightLevel);
                }
                lightQueue.push(topTile.GetCoords());
            }
        }
        //left tile
        if( IsValidCoords({coords.x - 1, coords.y}))
        {
            Tile &leftTile = UnsafeGetTile({coords.x - 1, coords.y});
            if(leftTile.lightLevel < tile.lightLevel)
            {
                if(leftTile.lightConsumption == 0)
                {
                    leftTile.SetLighting(tile.lightLevel - 2);
                }
                else
                {
                    leftTile.SetLighting(tile.lightLevel);
                }
                lightQueue.push(leftTile.GetCoords());
            }
        }
        //bottom tile
        if( IsValidCoords({coords.x, coords.y + 1}))
        {
            Tile &bottomTile = UnsafeGetTile({coords.x, coords.y + 1});
            if(bottomTile.lightLevel < tile.lightLevel)
            {
                if(bottomTile.lightConsumption == 0)
                {
                    bottomTile.SetLighting(tile.lightLevel - 2);
                }
                else
                {
                    bottomTile.SetLighting(tile.lightLevel);
                }
                lightQueue.push(bottomTile.GetCoords());
            }
        }
        //right tile
        if( IsValidCoords({coords.x + 1, coords.y}))
        {
            Tile &rightTile = UnsafeGetTile({coords.x + 1, coords.y});
            if(rightTile.lightLevel < tile.lightLevel)
            {
                if(rightTile.lightConsumption == 0)
                {
                    rightTile.SetLighting(tile.lightLevel - 2);
                }
                else
                {
                    rightTile.SetLighting(tile.lightLevel);
                }
                lightQueue.push(rightTile.GetCoords());
            }
        }
    }
    
}

void Map::PlaceOrePatch(Vector2 tileCoords, short oreType, float spawnChance, int &succesfulPositions)
{
    succesfulPositions = 0;
    for(int x = tileCoords.x ; x < tileCoords.x + 5; x++)
    {
        for(int y = tileCoords.y ; y < tileCoords.y + 5; y++)
        {
            Tile &tile = SafeGetTile({x, y});
            const float randomNumber = (float)rand() / RAND_MAX;
            if(randomNumber <= spawnChance && tile.type == Tile::STONE)
            {
                tile.SetProperties(oreType);
                tile.UpdateTextureRect(CheckTileIntersection({tileCoords.x, tileCoords.y}));
                succesfulPositions++;
            }
            else
            {
                spawnChance += 0.15 * spawnChance;
            }
        }
    }
    succesfulPositions /= 2;
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
    if(coords.x == 1 && coords.y == 7)
    {
        std::cout << "nice\n";
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