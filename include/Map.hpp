#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Map
{
public:
    Map();
public:
    void Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime);
    void Draw(sf::RenderWindow &window, Vector2 mousePos);
    void Generate();
    void SpawnPlayer();
    void Save();
    void Load();
private:
    void HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime);
    void HandleCollisions(float deltaTime);
    void UpdateSurroundingTiles(Vector2 centerTileCoords);
    bool PlaceTree(Vector2 rootCoords);
    void PlaceOrePatch(Vector2 tileCoords, short oreType, float spawnChance);
    Tile *Raycast(Vector2 startPosition, Vector2 targetPosition);
    short CheckTileIntersection(Vector2 coords);
    std::vector<Vector2> GetCollidableTilesCoords() const;
    std::vector<Vector2> GetBreakableTilesCoords() const;
    std::vector<Vector2> GetVisibleTilesCoords() const;
    std::vector<Vector2> GetPlayerBBTilesCoords() const;
    std::vector<Vector2> GetTreeTilesCoords(Vector2 treeTileCoords) const;
    Tile &SafeGetTile(Vector2 coords);
    Tile &UnsafeGetTile(Vector2 coords);
    const Tile &SafeGetTile(Vector2 coords) const;
    const Tile &UnsafeGetTile(Vector2 coords) const;
    bool IsValidCoords(Vector2 coords) const;
public:
    std::vector<Tile> tiles;
    Player player;
    sf::Texture backgroundTxt;
    char gameState {GS_MAP};
};

/*
    Returns a vector of tile coordinates within the grid, sorted in ascending order.
    The first element (vector[0]) contains {smallestX, smallestY}.
    The last element (vector[vector.size() - 1]) contains {biggestX, biggestY}.
*/
std::vector<Vector2>GetTileCoordsInArea(const std::vector<Tile> &map, Vector2 areaCenter, Vector2 areaSize);

// returns array of count float number between 0.0f and 1.0f
template <unsigned int Count>
std::array<float, Count> GenerateRandomArray(float min, float max)
{
    std::array<float, Count> arr;
    for(int i = 0; i < arr.size(); i++)
    {
        arr[i] = min + float(rand()) / RAND_MAX * (max - min);;
    }
    return arr;
};

/*
    octaves - increasy complexity of map
    bias - controls smoothness of terrain. Higher bias smoother terrain, lower bias rougher terrain  
*/
template<unsigned int Count>
std::array<float, Count> PerlinNoise1D(std::array<float, Count> seed, int octaves, float bias)
{
    std::array<float, Count> perlinNoise;
    for(int i = 0; i < perlinNoise.size(); i++)
    {
        float noise = 0.0f;
        float scaleAcc= 0.0f;
        float scale = 1.0f;

        for(int o = 0; o < octaves; o++)
        {
            // bitwise right shift effectively divides expression by power of 2
            int pitch = Count >> o;
            // int truncation so (i / pitch) * pitch won't give same result as just i
            int sample1 = (i / pitch) * pitch;
            // wrap around at array end by using '%'
            int sample2 = (sample1 + pitch) % Count;
            float blend = (float)(i - sample1) / (float)pitch;
            float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

            scaleAcc += scale;
            noise += sample * scale;
            scale /= bias;
        }
        perlinNoise[i] = noise / scaleAcc;
    }
    return perlinNoise;
}