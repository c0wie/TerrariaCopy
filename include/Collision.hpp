#pragma once
#include "Vector2.hpp"

inline bool PointRectCollision(Vector2 point, Vector2 topLeftCorner, Vector2 botRightCorner)
{
    return (topLeftCorner.x <= point.x && botRightCorner.x >= point.x 
            && topLeftCorner.y <= point.y && botRightCorner.y >= point.y);
}

template<typename T>
bool RayRectCollison(Vector2 rayStart, Vector2 rayDir, T target, Vector2 &contactPoint, Vector2 &contactNormal, float &contactTime) 
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
bool RectDynamicRectCollision(T &in, Y &target, Vector2 &contactPoint, Vector2 &contactNormal, float &contactTime, float deltaTime)
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