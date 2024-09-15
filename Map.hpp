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
    Player GetPlayer() const {return *m_Player.get(); }
private:
    void HandleKeyboardInput();
    void HandleCollision();
private:
    std::shared_ptr<Player> m_Player{std::make_shared<Player>(Player())};
    pe2d::PhysicsWorld m_PhysicsWorld{1U};
};
