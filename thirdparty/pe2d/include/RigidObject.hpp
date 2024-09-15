#pragma once
#include "Transform.hpp"
#include "Math.hpp"

namespace pe2d
{
    // class that holds information about object with dynamics
    class RigidObject
    {
    public:
        RigidObject() = default;
        RigidObject(size_t ID, pe2d::Vector2 size, Transform transform, float mass,
                    pe2d::Vector2 linearVelocity,float angularVelocity, pe2d::Vector2 gravity, bool isStatic, 
                    float staticFriction, float dynamicFriction, float restitution);
        RigidObject(size_t ID, pe2d::Vector2 size, Transform transform, float mass,
                    pe2d::Vector2 gravity, bool isStatic, float staticFriction, float dynamicFriction, float restitution);
        RigidObject(size_t ID, pe2d::Vector2 size, Transform transform, float mass, pe2d::Vector2 gravity, bool isStatic);
        virtual ~RigidObject() = default;
    public:
        constexpr unsigned int GetID() const { return m_ID; }
        pe2d::Vector2 GetSize() const { return m_Size; }
        //Returns collection of four vertices representing corners of a non rotated bounding box.
        std::array<pe2d::Vector2, 4> GetAABB() const;
        constexpr pe2d::Vector2 GetPosition() const { return m_Transform.position; }
        constexpr pe2d::Vector2 GetScale() const { return m_Transform.scale; }
        constexpr float GetRotation() const { return m_Transform.rotation; }
        constexpr Transform GetTransform() const { return m_Transform; }
        constexpr float GetMass() const { return m_Mass; }
        constexpr float GetInvMass() const 
        {
            if(m_IsStatic)
            {
                return 0.0f;
            }
            return (1.0f / m_Mass); 
        }
        constexpr pe2d::Vector2 GetLinearVelocity() const { return m_LinearVelocity; }
        constexpr float GetAngularVelocity() const { return m_AngularVelocity; }
        constexpr pe2d::Vector2 GetForce() const { return m_Force; }
        constexpr pe2d::Vector2 GetGravity() const { return m_Gravity; }
        constexpr float GetRotationalInertia() const { return m_RotationalInertia; }
        constexpr float GetInvRotationalInertia() const 
        {
            if(m_IsStatic)
            {
                return 0.0f;
            }
            return 1.0f / m_RotationalInertia;            
        }
        constexpr bool IsStatic() const { return m_IsStatic; }
        constexpr float GetStaticFriction() const { return m_StaticFriction; }
        constexpr float GetDynamicFriction() const { return m_DynamicFriction; }
        constexpr float GetRestitution() const { return m_Restitution; }

        constexpr void SetSize(pe2d::Vector2 size)
        {
            m_Size = size; 
            m_RotationalInertia = CalculateRotationalInertia();
        }
        constexpr void SetPosition(pe2d::Vector2 pos) { m_Transform.position = pos; }
        constexpr void Move(pe2d::Vector2 offset) { m_Transform.Move(offset); }
        constexpr void SetScale(pe2d::Vector2 scale) 
        { 
            scale.x = scale.x <= 0.0f ? m_Transform.scale.x : scale.x;
            scale.y = scale.y <= 0.0f ? m_Transform.scale.x : scale.y;
            m_Transform.scale = scale; 
        } 
        constexpr void SetRotation(float angleRadians) { m_Transform.rotation = angleRadians; }
        constexpr void Rotate(float angleRadians) { m_Transform.Rotate(angleRadians); }
        constexpr void SetTransform(Transform transform) { m_Transform = transform; }
        constexpr void SetMass(float mass)
        {
            mass = mass <= 0.0f? 0.1f : mass;
            m_Mass = m_IsStatic? pe2dMath::INF : mass;
            m_RotationalInertia = CalculateRotationalInertia();
        }
        constexpr void SetLinearVelocity(pe2d::Vector2 linearVelocity) { m_LinearVelocity = linearVelocity; }
        constexpr void AddLinearVelocity(pe2d::Vector2 linearVelocity) { m_LinearVelocity += linearVelocity; }
        constexpr void SetAngularVelocity(float angluarVelocity) { m_AngularVelocity = angluarVelocity; }
        constexpr void AddAngularVelocity(float angularVelocity) { m_AngularVelocity += angularVelocity; }
        constexpr void SetForce(pe2d::Vector2 force) { m_Force = force; }
        constexpr void AddForce(pe2d::Vector2 force) { m_Force += force; }
        constexpr void SetGravity(pe2d::Vector2 gravity) { m_Gravity = gravity; }
        constexpr void SetStaticFriction(float staticFriction)
        {
            std::clamp(staticFriction, 0.0f, 1.0f);
            m_StaticFriction = staticFriction;
        }
        constexpr void SetDynamicFriction(float dynamicFriction)
        {  
            std::clamp(dynamicFriction, 0.0f, 1.0f);
            m_DynamicFriction = dynamicFriction;
        }
        constexpr void SetRestitution(float restitution)
        {
            std::clamp(restitution, 0.0f, 1.0f);
            m_Restitution = restitution;
        }
    protected:
        constexpr float CalculateRotationalInertia()
        {
            if(m_IsStatic)
            {
                return pe2dMath::INF;
            }

            const float width = GetSize().x;
            const float height = GetSize().y;
            return (1.0f / 12.0f) * m_Mass * (width * width + height * height);
        }
    private:
        Transform m_Transform;
        pe2d::Vector2 m_LinearVelocity{0.0f, 0.0f};
        pe2d::Vector2 m_Force{0.0f, 0.0f};
        pe2d::Vector2 m_Gravity{0.0f, 0.0f};
        pe2d::Vector2 m_Size{50.0f, 50.0f};
        size_t m_ID {0U};
        float m_Mass{10.0f};
        float m_AngularVelocity{0.0f}; 
        float m_RotationalInertia{0.0f};
        float m_StaticFriction{0.0f};     // Static friction coefficient [in range 0 - 1] 
        float m_DynamicFriction{0.0f};    // Dynamic friction coefficient [in range 0 - 1]
        float m_Restitution{0.0f};        // Elasticy of collision [in range 0 - 1] 
        bool m_IsStatic{false};
    };
}