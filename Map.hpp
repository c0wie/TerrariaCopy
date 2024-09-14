#include "Global.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "PhysicsWorld.hpp"
#include <SFML/Graphics.hpp>


class Map
{
public:
    Map();
public:
    void Update(float deltaTime);
    void Draw(sf::RenderWindow &window) const;
    pe2d::RigidObject GetPlayer() { return m_PhysicsWorld.At(21372137U); }
private:
    void HandleKeyboardInput();
    void HandleCollision();
private:
    pe2d::PhysicsWorld m_PhysicsWorld{1U};
    std::vector<Tile> m_TiledWorld;
};
