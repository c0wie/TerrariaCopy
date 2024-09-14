#pragma once

#include "RigidObject.hpp"
#include "CollisionPoints.hpp"

namespace pe2d
{
    /*
        Struct holding general information about collision:
        - `ObjectA` and `ObjectB`: objects involved in collision
        - `Points`: sturct with detailed information obout collision
    */
    struct Collision
    {
    public:
        Collision(RigidObject &a, RigidObject &b, const CollisionPoints &points) :
            m_Points(points), m_ObjectA(&a), m_ObjectB(&b) {}
    public:
        RigidObject& GetObjectA() { return *m_ObjectA; }
        RigidObject& GetObjectB() { return *m_ObjectB; }
        CollisionPoints& GetCollisionPoints() { return m_Points; }
    private: 
        CollisionPoints m_Points;
        RigidObject *m_ObjectA;
        RigidObject *m_ObjectB;
    };
}