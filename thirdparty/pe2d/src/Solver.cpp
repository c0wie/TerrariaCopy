#include "Solver.hpp"
#include "Algo.hpp"

namespace pe2d
{
    void PositionSolver(std::vector<Collision> &collisions, float deltaTime)
    {
        for(size_t i = 0; i < collisions.size(); i++)
        {
            std::shared_ptr<RigidObject> rigidObjectA = collisions[i].GetObjectA();
            std::shared_ptr<RigidObject> rigidObjectB = collisions[i].GetObjectB();
            const auto &points = collisions[i].GetCollisionPoints();
            const Vector2 normal = points.Normal;
            
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
}