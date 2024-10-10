#pragma once
#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Map
{
public:
    Map(bool generateMap);
    ~Map();
public:
    void Update(Vector2 mousePos, bool isRelased, float deltaTime);
    void Draw(sf::RenderWindow &window) const;
private:
    void HandleMouseInput(Vector2 mousePos, float deltaTime);
    void HandleCollision(float deltaTime);
    std::array<Vector2, 20> GetCollidableTilesCoords(Vector2 position, Vector2 size) const;
    std::vector<Vector2> GetBreakableTilesCoords(Vector2 position, Vector2 size) const;
    std::vector<int> GetTilesToDraw(Vector2 playerPosition) const;
    std::pair<Vector2, Vector2> GetPlayerBoundingBox() const;
    void Save();
    void Load();
    void Generate(const std::array<float, MAP_WIDTH> &seed);
public:
    Player player;
    std::array<Tile, MAP_WIDTH * MAP_HEIGHT> tiles{};
};

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