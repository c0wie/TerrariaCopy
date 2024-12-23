#include "Collision.hpp"


bool RayRectCollison(Vector2 rayStart, Vector2 rayDir, const Tile &target, Vector2 &contactPoint,
                    Vector2 &contactNormal, float &contactTime) 
{   
    Vector2 nearIntersection = ((target.GetPosition() - target.GetSize() / 2.0f)- rayStart) / rayDir;
    Vector2 farIntersection = ((target.GetPosition() + target.GetSize() / 2.0f) - rayStart) / rayDir;

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

bool RectDynamicRectCollision(const Player &player, const Tile &tile, Vector2 &contactPoint,
                            Vector2 &contactNormal, float &contactTime, float deltaTime)
{
    if(player.velocity == Vector2{0.0f, 0.0f})
    {
        return false;
    }

    Tile expandedTarget;
    expandedTarget.SetPosition(tile.GetPosition());
    expandedTarget.SetSize(tile.GetSize() + player.size);

    if(RayRectCollison(player.position, player.velocity * deltaTime, expandedTarget, contactPoint, contactNormal, contactTime))
    {
        return (contactTime >= 0.0f && contactTime < 1.0f);
    }
    return false;
}
