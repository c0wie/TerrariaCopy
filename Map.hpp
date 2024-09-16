#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include <SFML/Graphics.hpp>

constexpr float fabs(float value)
{
    if(value < 0.0f)
    {
        value *= -1.0f;
    }
    return value;
}

bool CollisionSolver(Tile &tile, Player &player, sf::Vector2f &direction);

class Map
{
public:
    Map();
public:
    void Update(float deltaTime);
    void Draw(sf::RenderWindow &window) const;
private:
    void HandleKeyboardInput();
    void OnCollision(sf::Vector2f &direction);
public:
    Player player;
    std::vector<Tile> tiles;
};
