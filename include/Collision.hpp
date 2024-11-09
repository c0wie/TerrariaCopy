#pragma once
#include "Tile.hpp"
#include "Player.hpp"
#include "Vector2.h"

constexpr bool PointRectCollision(Vector2 point, Vector2 topLeftCorner, Vector2 botRightCorner)
{
    return (topLeftCorner.x <= point.x && botRightCorner.x >= point.x 
            && topLeftCorner.y <= point.y && botRightCorner.y >= point.y);
}

bool RayRectCollison(Vector2 rayStart, Vector2 rayDir, const Tile &target, Vector2 &contactPoint,
                    Vector2 &contactNormal, float &contactTime);

bool RectDynamicRectCollision(const Player &player, const Tile &tile, Vector2 &contactPoint,
                            Vector2 &contactNormal, float &contactTime, float deltaTime);