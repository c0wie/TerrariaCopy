#include "RigidObject.hpp"
#include "Algo.hpp"

namespace pe2d
{
    RigidObject::RigidObject(size_t ID, Vector2 size, Transform transform, float mass,
                            Vector2 linearVelocity, float angularVelocity, Vector2 gravity, bool isStatic, 
                            float staticFriction, float dynamicFriction, float restitution) :
        m_Transform(transform),
        m_LinearVelocity(linearVelocity),
        m_Force({0.0f, 0.0f}),
        m_Gravity(gravity),
        m_Size(size),
        m_ID(ID),
        m_AngularVelocity(angularVelocity),
        m_IsStatic(isStatic)
    {  
        SetMass(mass);
        SetStaticFriction(staticFriction);
        SetDynamicFriction(dynamicFriction);
        SetRestitution(restitution);
    }

    RigidObject::RigidObject(size_t ID, Vector2 size, Transform transform, float mass,
                            Vector2 gravity, bool isStatic, float staticFriction, float dynamicFriction, float restitution):
        m_Transform(transform),
        m_LinearVelocity({0.0f, 0.0f}),
        m_Force({0.0f, 0.0f}),
        m_Gravity(gravity),
        m_Size(size),
        m_ID(ID), 
        m_AngularVelocity(0.0f),
        m_IsStatic(isStatic)
    {
        SetMass(mass);
        SetStaticFriction(staticFriction);
        SetDynamicFriction(dynamicFriction);
        SetRestitution(restitution);
    }

    RigidObject::RigidObject(size_t ID, Vector2 size, Transform transform, float mass, Vector2 gravity, bool isStatic) :
        m_Transform(transform),
        m_LinearVelocity({0.0f, 0.0f}),
        m_Force({0.0f, 0.0f}),
        m_Gravity({gravity}),
        m_Size(size),
        m_ID(ID),
        m_AngularVelocity(0.0f),
        m_StaticFriction(0.0f),
        m_DynamicFriction(0.0f),
        m_Restitution(0.0f),
        m_IsStatic(isStatic)
    {
        SetMass(mass);
    }

    std::array<Vector2, 4> RigidObject::GetAABB() const
    {
        const std::array<Vector2, 4> vertices = algo::GetBoxVertices(GetSize(), m_Transform);

        Vector2 topLeftCorner = Vector2(pe2dMath::INF, pe2dMath::INF);
        Vector2 botRightCorner = Vector2(pe2dMath::MIN, pe2dMath::MIN);
        for(const Vector2 vertex : vertices)
        {
            topLeftCorner.x = std::min(topLeftCorner.x, vertex.x);
            topLeftCorner.y = std::min(topLeftCorner.y, vertex.y);
            botRightCorner.x = std::max(botRightCorner.x, vertex.x);    
            botRightCorner.y = std::max(botRightCorner.y, vertex.y);
        }
        const Vector2 topRightCorner = Vector2(botRightCorner.x, topLeftCorner.y);
        const Vector2 botLeftCorner = Vector2(topLeftCorner.x, botRightCorner.y);
        
        return {topLeftCorner, topRightCorner, botRightCorner, botLeftCorner};
    }
}
