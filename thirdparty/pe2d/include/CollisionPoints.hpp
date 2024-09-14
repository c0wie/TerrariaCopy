#pragma once

#include "Vector2.hpp"

namespace pe2d
{
    /*
        Struct holding detailed information about collision:
        - `Depth`: The depth of collison.
        - `Normal`: The normal vector along which collision occured.
        - `ContactPoint1`: The first contact point of the collison.
        - `ContactPoint2`: The second contact point of the collision, if applicable.
        - `ContactCount`: The count of valid contact points.
        - `HasCollision`: A boolean indicating whether a collision actually occured.
        The second contact point (`ContactPoint2`) may be invalid or unused in some cases,
        so `contactCount` should be used to determine how many contact points are considered valid.
    */
    struct CollisionPoints
    {
    public:
        CollisionPoints() = default;
        CollisionPoints(Vector2 normal, float depth, Vector2 contactPoint1, Vector2 contactPoint2, bool hasCollision) :
            Normal(normal), ContactPoint1(contactPoint1), ContactPoint2(contactPoint2),
            Depth(depth), ContactCount(2), HasCollision(hasCollision)
        {}
        CollisionPoints(Vector2 normal, float depth, Vector2 contactPoint1, bool hasCollision) :
            Normal(normal), ContactPoint1(contactPoint1), ContactPoint2({0.0f, 0.0f}),
            Depth(depth), ContactCount(1), HasCollision(hasCollision)
        {}
    public:
        Vector2 Normal{0.0f, 0.0f}; // normalized length of vector B-A
        Vector2 ContactPoint1{0.0f, 0.0f};
        Vector2 ContactPoint2{0.0f, 0.0f};
        float Depth{0.0f}; // length of overlap beetwen two objects
        int ContactCount{0};
        bool HasCollision{false};
    };
}