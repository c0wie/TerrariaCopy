#include "PhysicsWorld.hpp"
#include "Algo.hpp"

namespace pe2d
{
    void PhysicsWorld::Step(float deltaTime)
    {
        for(int i = 0; i < m_Substeps; i++)
        {
            const float subtime = deltaTime / (float)m_Substeps;
            ApplyGravity();
            ResolveCollisions(subtime);
            MoveObjects(subtime);
        }
    }

    void PhysicsWorld::AddObject(std::shared_ptr<RigidObject> object)
    {
        const unsigned int ID = object->GetID();
        if(m_Objects.find(ID) != m_Objects.end())
        {
            ASSERT("Can't assign same ID to more than one object in CollisionWorld");
        }
        m_Objects[ID] = object;
    }

    std::unordered_map<size_t, std::shared_ptr<RigidObject>>::iterator PhysicsWorld::RemoveObject(size_t ID)
    {
        auto it = m_Objects.find(ID);
        if (it != m_Objects.end()) 
        {
            return m_Objects.erase(it); 
        }
        return m_Objects.end(); 
    }

    std::shared_ptr<RigidObject> PhysicsWorld::At(unsigned int ID)
    {
        if(m_Objects.find(ID) == m_Objects.end())
        {
            ASSERT("Uvalid index acces");
        }
        return m_Objects.at(ID);
    }

    void PhysicsWorld::ResolveCollisions(float deltaTime)
    {
        std::vector<Collision> collisions;
        collisions.reserve(m_Objects.size());
        if(m_IsGridOn)
        {
            m_Grid.Update(m_Objects);
            std::list<std::pair<size_t, size_t>> pairs = m_Grid.GetCollisionPairs();
            for(auto it = pairs.begin(); it != pairs.end(); it++)
            {
                FindCollisions(it->first, it->second, collisions);
            }
        }
        else
        {
            for(auto itA = m_Objects.begin(); itA != m_Objects.end(); itA++)
            {
                for(auto itB = m_Objects.begin(); itB != m_Objects.end(); itB++)
                {
                    if(itA == itB)
                    {
                        break;
                    }
                    FindCollisions(itA->first, itB->first, collisions);
                }
            }
        }
        m_Solver(collisions, deltaTime);
    }
    
    void PhysicsWorld::AddGrid(Vector2 topLeftCorner, Vector2 bottomRightCorner, float cellSize)
    {
        if(m_IsGridOn)
        {
            return;
        }
        m_Grid = Grid(topLeftCorner, bottomRightCorner, cellSize);
        m_IsGridOn = true;
    }

    void PhysicsWorld::RemoveGrid()
    {
        if(!m_IsGridOn)
        {
            return;
        }
        m_Grid = Grid();
        m_IsGridOn = false;
    }

    void PhysicsWorld::ResizeGrid(Vector2 topLeftCorner, Vector2 bottomRightCorner, float cellSize)
    {
        m_Grid = Grid(topLeftCorner, bottomRightCorner, cellSize);
    }

    void PhysicsWorld::FindCollisions(size_t IDA, size_t IDB, std::vector<Collision> &collisions)
    {
        std::shared_ptr<RigidObject> A = m_Objects.at(IDA);
        std::shared_ptr<RigidObject> B = m_Objects.at(IDB);

        if(A->IsStatic() && B->IsStatic())
        {
            return;
        }

        // check if objects are penetrating if so pull them apart
        CollisionPoints points = algo::FindBoxBoxCollision(A->GetSize(), A->GetTransform(), B->GetSize(), B->GetTransform());
        if(points.HasCollision)
        {
            collisions.emplace_back(A, B, points);
        }
    }

    void PhysicsWorld::ApplyGravity()
    {
        for(auto it = m_Objects.begin(); it != m_Objects.end(); it++)
        {
            auto object = it->second;
            if(object->IsStatic())
            {
                continue;
            }
            object->AddForce( object->GetGravity() * object->GetMass() );
        }
    }

    void PhysicsWorld::MoveObjects(float deltaTime)
    {
        for(auto it = Begin(); it != End(); it++)
        {
            std::shared_ptr<RigidObject> object = it->second;
            if(object->IsStatic())
            {
                continue;
            }
            const Vector2 acceleration = object->GetForce() * object->GetInvMass();
            const Vector2 newVel = object->GetLinearVelocity() + acceleration * deltaTime;
            object->Move(object->GetLinearVelocity() * deltaTime + (acceleration * deltaTime * deltaTime * 0.5));
            object->Rotate(object->GetAngularVelocity() * deltaTime);
            object->SetLinearVelocity(newVel);   
            object->SetForce({0.0f, 0.0f});
        }
    }
}