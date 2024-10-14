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
    void Update(Vector2 mousePos, bool isRelased, float deltaTime);
    void Draw(sf::RenderWindow &window) const;
    void Load();
    void Generate();
    void Save();
private:
    void HandleMouseInput(Vector2 mousePos, float deltaTime);
    void HandleCollisions(float deltaTime);
    std::vector<Vector2> GetCollidableTilesCoords() const;
    std::vector<Vector2> GetBreakableTilesCoords() const;
    std::vector<Vector2> GetVisibleTilesCoords() const;
    std::vector<Vector2> GetPlayerBBTilesCoords() const;
    void UpdateSurroundingTiles(Vector2 centerTileCoords);
    short CheckTileIntersection(Vector2 coords);
    void PlaceTree(Vector2 rootCoords, short rootType);
public:
    std::array<Tile, MAP_WIDTH * MAP_HEIGHT> tiles{};
    Player player;
    sf::Texture backgroundTxt;
};

/*
    Returns a vector of tile coordinates within the grid, sorted in ascending order.
    The first element (vector[0]) contains {smallestX, smallestY}.
    The last element (vector[vector.size() - 1]) contains {biggestX, biggestY}.
*/
std::vector<Vector2>GetTileCoordsInArea(const std::array<Tile, MAP_WIDTH * MAP_HEIGHT> &map, Vector2 areaCenter, Vector2 areaSize);


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

Vector2 CalculateMouseCoords(Vector2 mousePos);

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