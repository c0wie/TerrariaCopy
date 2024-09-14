#include "Map.hpp"
#include <cmath>

Map::Map()
{
    m_PhysicsWorld.AddGrid({-1000.0f, -1000.0f}, {1000.0f, 1000.0f}, 100.0f);
}

void Map::Draw(sf::RenderWindow &window) const
{
    for(auto it = m_PhysicsWorld.cBegin(); it != m_PhysicsWorld.cEnd(); it++)
    {
    } 
}