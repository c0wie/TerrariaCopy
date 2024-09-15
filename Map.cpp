#include "Map.hpp"
#include <cmath>

void GameSolver(std::vector<Collision> &collisions, float deltaTime)
{
    for(size_t i = 0; i < collisions.size(); i++)
    {
        std::shared_ptr<RigidObject> rigidObjectA = collisions[i].GetObjectA();
        std::shared_ptr<RigidObject> rigidObjectB = collisions[i].GetObjectB();
        const auto &points = collisions[i].GetCollisionPoints();
        const Vector2 normal = points.Normal;
        
        std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(rigidObjectB);
        if(player && points.Normal == Vector2{-0.0f, -1.0f})
        {
            player->SetCanJump(true);
        }
        
        Vector2 MTV = normal * points.Depth;
        if(pe2dMath::Dot(MTV, rigidObjectA->GetPosition() - rigidObjectB->GetPosition()) < 0.0f)
        {
            MTV *= -1.0f;
        }
        
        if (rigidObjectA->IsStatic())
        {
            rigidObjectB->Move(-1.0f * MTV);
        }
        else if (rigidObjectB->IsStatic())
        {
            rigidObjectA->Move(MTV);
        }
        else
        {
            rigidObjectA->Move(MTV / 2.0f);
            rigidObjectB->Move(MTV / -2.0f);
        }
    }
}

Map::Map()
{
    m_PhysicsWorld.SetSolver(GameSolver);
    m_PhysicsWorld.AddGrid({-1000.0f, -1000.0f}, {1000.0f, 1000.0f}, 100.0f);
    m_PhysicsWorld.AddObject(m_Player);
    m_PhysicsWorld.AddObject(std::make_shared<Tile>(Tile(m_PhysicsWorld.Size(), {SCREEN_WIDTH / 2.0f, 800.0f}, tileType::STONE)));
}

void Map::Update(float deltaTime)
{
    m_Player->SetLinearVelocity({0.0f, m_Player->GetLinearVelocity().y});
    HandleKeyboardInput();
    m_PhysicsWorld.Step(deltaTime);
}

void Map::Draw(sf::RenderWindow &window) const
{
    for(auto it = m_PhysicsWorld.cBegin(); it != m_PhysicsWorld.cEnd(); it++)
    {
        std::shared_ptr<Tile> tile = std::dynamic_pointer_cast<Tile>(it->second);
        if(tile)
        {   
            sf::RectangleShape shape(tile->GetSize());
            shape.setOrigin(shape.getSize() / 2.0f);
            shape.setFillColor(tile->color);
            shape.setPosition(tile->GetPosition());
            window.draw(shape);
        }
        else
        {
            auto body = it->second;
            sf::RectangleShape player(body->GetSize());
            player.setOrigin(player.getSize() / 2.0f);
            player.setFillColor(sf::Color::Red);
            player.setPosition(body->GetPosition());
            window.draw(player);
        }
    } 
}

void Map::HandleKeyboardInput()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        m_Player->AddLinearVelocity({-m_Player->GetSpeed(), 0.0f});
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        m_Player->AddLinearVelocity({m_Player->GetSpeed(), 0.0f});
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_Player->CanJump())
    {
        m_Player->SetCanJump(false);
        m_Player->SetLinearVelocity({m_Player->GetLinearVelocity().x, -sqrtf(2.0f * GRAVITY * m_Player->GetJumpHeight())});
    }
}