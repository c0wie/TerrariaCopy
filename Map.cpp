#include "Map.hpp"
#include <cmath>

Map::Map()
{
    m_PhysicsWorld.AddGrid({-1000.0f, -1000.0f}, {1000.0f, 1000.0f}, 100.0f);
    m_PhysicsWorld.AddObject(pe2d::RigidObject(21372137U, {50.0f, 100.0f}, pe2d::Transform({0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f), 10.0f, {0.0f, GRAVITY}, false));
    m_PhysicsWorld.AddObject(pe2d::RigidObject(1U, {50.0f, 50.0f}, pe2d::Transform({0.0f, 400.0f}, {1.0f, 1.0f}, 0.0f), 10.0f, {0.0f, GRAVITY}, true));
}

void Map::Draw(sf::RenderWindow &window) const
{
    for(auto it = m_PhysicsWorld.cBegin(); it != m_PhysicsWorld.cEnd(); it++)
    {
        auto object = it->second;
        sf::RectangleShape player(object.GetSize());
        player.setOrigin(player.getSize() / 2.0f);
        player.setFillColor(sf::Color::Red);
        player.setPosition(object.GetPosition());
        window.draw(player);
    } 
}

void Map::Update(float deltaTime)
{
    m_PhysicsWorld.Step(deltaTime);
}