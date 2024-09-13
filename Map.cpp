#include "Map.hpp"
#include <cmath>

void SolveCollsion(Player &player, const Tile &tile)
{
    const sf::Vector2f playerCenter = player.GetPosition();
    const sf::Vector2f tileCenter = tile.position;
    const sf::Vector2f playerHalfSize = player.GetSize() / 2.0f;
    const sf::Vector2f tileHalfSize = tile.size / 2.0f;
    const sf::Vector2f positionDelta = tileCenter - playerCenter;
    const sf::Vector2f intersection = sf::Vector2f{ abs(positionDelta.x) - (playerHalfSize.x + tileHalfSize.x), 
                                                    abs(positionDelta.y) - (playerHalfSize.y + tileHalfSize.y) };
    
    if(intersection.x < 0.0f && intersection.y < 0.0f)
    {
        if(intersection.x > intersection.y)
        {
            if(positionDelta.x > 0.0f)
            {
                player.Move({intersection.x, 0.0f});
            }
            else
            {
                player.Move({-intersection.x, 0.0f});
            }
        }
        else
        {   
            if(positionDelta.y > 0.0f)
            {
                player.SetVelocity({player.GetVelocity().x, 0.0f});
                player.Move({0.0f, intersection.y});
                player.SetCanJump(true);
            }
            else
            {
                player.Move({0.0f, -intersection.y});
            }
        }
    }
}

Map::Map(int32_t size)
{
    m_TiledMap.reserve(size);
    for(int32_t i = 0; i < size; i++)
    {
        m_TiledMap.emplace_back(sf::Vector2f(i * 50.0f, 875.0f));
    }
}

void Map::Update(float deltaTime)
{
    m_Player.SetVelocity({0.0f, m_Player.GetVelocity().y});
    m_Player.SetSprintState(false);
    if(!m_Player.GetCanJump())
    {
        m_Player.AddVelocity({0.0f, GRAVITY * deltaTime});
    }
    HandleKeyboardInput();
    HandleCollision();
    m_Player.Move(m_Player.GetVelocity() * deltaTime);
}

void Map::Draw(sf::RenderWindow &window) const
{
    m_Player.Draw(window);
    for(int32_t i = 0; i < m_TiledMap.size(); i++)
    {
        m_TiledMap[i].Draw(window);
    }
}

void Map::HandleKeyboardInput()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        m_Player.SetSprintState(true);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        m_Player.AddVelocity({-m_Player.GetSpeed(), 0.0f});
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        m_Player.AddVelocity({m_Player.GetSpeed(), 0.0f});
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_Player.GetCanJump())
    {
        m_Player.SetCanJump(false);
        m_Player.AddVelocity({0.0f, -sqrtf(2.0f * GRAVITY * m_Player.GetJumpHeight())});
    }
}

void Map::HandleCollision()
{
    for(int32_t i = 0; i < m_TiledMap.size(); i++)
    {
        SolveCollsion(m_Player, m_TiledMap[i]);
    }
}
