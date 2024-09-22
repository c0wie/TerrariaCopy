#pragma once
#include "Vector2.hpp"

struct Rect
{
    Vector2 pos;
    Vector2 size;
    Vector2 velocity;
};

inline bool PointRectCollision(const Vector2 p, const Rect r)
{
    return (p.x < (r.pos.x + r.size.x / 2.0f) && p.x > (r.pos.x - r.size.x / 2.0f)
            && p.y < (r.pos.y + r.size.y / 2.0f) && p.y > (r.pos.y - r.size.y / 2.0f));
}

inline bool RectRectCollision(const Rect &r1, const Rect &r2)
{
    return (r1.pos.x - r1.size.x / 2.0f < r2.pos.x + r2.size.x / 2.0f && 
            r1.pos.x + r1.size.x / 2.0f > r2.pos.x -r2.size.x / 2.0f && 
            r1.pos.y - r1.size.y / 2.0f < r2.pos.y + r2.size.y / 2.0f && 
            r1.pos.y + r1.size.y / 2.0f > r2.pos.y -r2.size.y / 2.0f);
}

template<typename T>
inline bool RayRectCollison(Vector2 rayStart, Vector2 rayDir, T target, Vector2 &contactPoint, Vector2 &contactNormal, float &contactTime) 
{   
    Vector2 nearIntersection = ((target.position - target.size / 2.0f) - rayStart) / rayDir;
    Vector2 farIntersection = ((target.position + target.size / 2.0f) - rayStart) / rayDir;

    if(std::isnan(nearIntersection.x) || std::isnan(nearIntersection.y))
    {
        return false;
    }
    if(std::isnan(farIntersection.x) || std::isnan(farIntersection.y))
    {
        return false;
    }

    // sort distances
    if(nearIntersection.x > farIntersection.x)
    {
        std::swap(nearIntersection.x, farIntersection.x);
    }
    if(nearIntersection.y > farIntersection.y)
    {
        std::swap(nearIntersection.y, farIntersection.y);
    }

    // early rejection
    if(nearIntersection.x > farIntersection.y || nearIntersection.y > farIntersection.x)
    {
        return false;
    }

    contactTime = std::max(nearIntersection.x, nearIntersection.y);
    float farIntersectionTime = std::min(farIntersection.x, farIntersection.y);

    if(farIntersectionTime < 0.0f)
    {
        return false;
    }

    // parametric line equation
    contactPoint = rayStart + contactTime * rayDir;

    if(nearIntersection.x > nearIntersection.y)
    {
        if(rayDir.x < 0.0f)
        {
            contactNormal = {1.0f, 0.0f};
        }
        else
        {
            contactNormal = {-1.0f, 0.0f};
        }
    }
    else if (nearIntersection.x < nearIntersection.y)
    {
        if(rayDir.y < 0.0f)
        {
            contactNormal = {0.0f, 1.0f};
        }
        else
        {
            contactNormal = {0.0f, -1.0f};
        }
    }
    return true;
}

template<typename T, typename Y>
inline bool RectDynamicRectCollision(T &in, Y &target, Vector2 &contactPoint, Vector2 &contactNormal, float &contactTime, float deltaTime)
{
    if(in.velocity.x == 0.0f && in.velocity.y == 0.0f)
    {
        return false;
    }

    Y expandedTarget;
    expandedTarget.position = target.position;
    expandedTarget.size = target.size + in.size;

    if(RayRectCollison(in.position, in.velocity * deltaTime, expandedTarget, contactPoint, contactNormal, contactTime))
    {
        return (contactTime >= 0.0f && contactTime < 1.0f);
    }
    return false;
}