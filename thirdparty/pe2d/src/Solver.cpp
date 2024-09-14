#include "Solver.hpp"
#include "Algo.hpp"

namespace pe2d
{
    void PositionSolver(std::vector<Collision> &collisions, float deltaTime)
    {
        for(size_t i = 0; i < collisions.size(); i++)
        {
            RigidObject &rigidObjectA = collisions[i].GetObjectA();
            RigidObject &rigidObjectB = collisions[i].GetObjectB();
            const auto &points = collisions[i].GetCollisionPoints();
            const Vector2 normal = points.Normal;
            
            Vector2 MTV = normal * points.Depth;
            if(pe2dMath::Dot(MTV, rigidObjectA.GetPosition() - rigidObjectB.GetPosition()) < 0.0f)
            {
                MTV *= -1.0f;
            }
            
            if (rigidObjectA.IsStatic())
            {
                rigidObjectB.Move(-1.0f * MTV);
            }
            else if (rigidObjectB.IsStatic())
            {
                rigidObjectA.Move(MTV);
            }
            else
            {
                rigidObjectA.Move(MTV / 2.0f);
                rigidObjectB.Move(MTV / -2.0f);
            }
        }
    }
    
    void ImpulseSolverWithoutFriction(std::vector<Collision> &collisions, float deltaTime)
    {
        for(auto &collision : collisions)
        {
            RigidObject &rigidObjectA = collision.GetObjectA();
            RigidObject &rigidObjectB = collision.GetObjectB();
            const CollisionPoints &points = collision.GetCollisionPoints();
            const float invMassA = rigidObjectA.GetInvMass();
            const float invMassB = rigidObjectB.GetInvMass();
            const float invInertiaA = rigidObjectA.GetInvRotationalInertia();
            const float invInertiaB = rigidObjectB.GetInvRotationalInertia();
            const Vector2 normal = points.Normal;
            const int contactCount = points.ContactCount;
            const float coefficientOfRestitution = (rigidObjectA.GetRestitution() + rigidObjectB.GetRestitution()) * 0.5f;
            const float coefficientOfStaticFriction = (rigidObjectA.GetStaticFriction() + rigidObjectB.GetStaticFriction()) * 0.5f;
            const float coefficientOfDynamicFriction = (rigidObjectA.GetDynamicFriction() + rigidObjectB.GetDynamicFriction()) * 0.5f;
            const Vector2 contactList[2] = { points.ContactPoint1, points.ContactPoint2 };
            std::array<Vector2, 2> rAList, rBList, impulseList, frictionImpulseList;
            std::array<float, 2> jList;

            Vector2 MTV = normal * points.Depth;
            if(pe2dMath::Dot(MTV, rigidObjectA.GetPosition() - rigidObjectB.GetPosition()) < 0.0f)
            {
                MTV *= -1.0f;
            }
            if (rigidObjectA.IsStatic())
            {
                rigidObjectB.Move(-1.0f * MTV);
            }
            else if (rigidObjectB.IsStatic())
            {
                rigidObjectA.Move(MTV);
            }
            else
            {
                rigidObjectA.Move(MTV / 2.0f);
                rigidObjectB.Move(MTV / -2.0f);
            }


#pragma region add impulse
            for(int i = 0; i < contactCount; i++)
            {
                // vector pointing from center of mass of the objects to the contact points
                rAList[i] = contactList[i] - rigidObjectA.GetPosition();
                rBList[i] = contactList[i] - rigidObjectB.GetPosition();
                
                const Vector2 rAperp = pe2dMath::Perp(rAList[i]);
                const Vector2 rBperp = pe2dMath::Perp(rBList[i]);

                const Vector2 relativeVelocity = (rigidObjectA.GetLinearVelocity() + (rAperp * rigidObjectA.GetAngularVelocity()) ) 
                                            - (rigidObjectB.GetLinearVelocity() + (rBperp * rigidObjectB.GetAngularVelocity()) ); 

                const float relativeVelocityAlongNormal = pe2dMath::Dot(relativeVelocity, normal);

                const float rAperpNormal = pe2dMath::Dot(rAperp, normal);
                const float rBperpNormal = pe2dMath::Dot(rBperp, normal);

                const float denominator = invMassA + invMassB 
                    + (rAperpNormal * rAperpNormal) * invInertiaA
                    + (rBperpNormal * rBperpNormal) * invInertiaB;

                float j = -(1.0f + coefficientOfRestitution) * relativeVelocityAlongNormal;
                j /= denominator;
                j /= (float)contactCount;

                impulseList[i] = j * normal;
            }   
            for(int i = 0; i < contactCount; i++)
            {
                auto impulse = impulseList[i];
                rigidObjectA.AddLinearVelocity(impulse * invMassA);
                rigidObjectA.AddAngularVelocity(pe2dMath::Cross(rAList[i], impulse) * invInertiaA);
                rigidObjectB.AddLinearVelocity(impulse * -invMassB);
                rigidObjectB.AddAngularVelocity(pe2dMath::Cross(rBList[i], impulse) * -invInertiaB);
            }
#pragma endregion
        }
    }

    void ImpulseSolverWithFriction(std::vector<Collision> &collisions, float deltaTime)
    {
        for(auto &collision : collisions)
        {
            RigidObject &rigidObjectA = collision.GetObjectA();
            RigidObject &rigidObjectB = collision.GetObjectB();
            const CollisionPoints &points = collision.GetCollisionPoints();
            const float invMassA = rigidObjectA.GetInvMass();
            const float invMassB = rigidObjectB.GetInvMass();
            const float invInertiaA = rigidObjectA.GetInvRotationalInertia();
            const float invInertiaB = rigidObjectB.GetInvRotationalInertia();
            const Vector2 normal = points.Normal;
            const int contactCount = points.ContactCount;
            const float coefficientOfRestitution = (rigidObjectA.GetRestitution() + rigidObjectB.GetRestitution()) * 0.5f;
            const float coefficientOfStaticFriction = (rigidObjectA.GetStaticFriction() + rigidObjectB.GetStaticFriction()) * 0.5f;
            const float coefficientOfDynamicFriction = (rigidObjectA.GetDynamicFriction() + rigidObjectB.GetDynamicFriction()) * 0.5f;
            const Vector2 contactList[2] = { points.ContactPoint1, points.ContactPoint2 };
            std::array<Vector2, 2> rAList, rBList, impulseList, frictionImpulseList;
            std::array<float, 2> jList;

            Vector2 MTV = normal * points.Depth;
            if(pe2dMath::Dot(MTV, rigidObjectA.GetPosition() - rigidObjectB.GetPosition()) < 0.0f)
            {
                MTV *= -1.0f;
            }
            if (rigidObjectA.IsStatic())
            {
                rigidObjectB.Move(-1.0f * MTV);
            }
            else if (rigidObjectB.IsStatic())
            {
                rigidObjectA.Move(MTV);
            }
            else
            {
                rigidObjectA.Move(MTV / 2.0f);
                rigidObjectB.Move(MTV / -2.0f);
            }


#pragma region add impulse
            for(int i = 0; i < contactCount; i++)
            {
                // vector pointing from center of mass of the objects to the contact points
                rAList[i] = contactList[i] - rigidObjectA.GetPosition();
                rBList[i] = contactList[i] - rigidObjectB.GetPosition();
                
                const Vector2 rAperp = pe2dMath::Perp(rAList[i]);
                const Vector2 rBperp = pe2dMath::Perp(rBList[i]);

                const Vector2 relativeVelocity = (rigidObjectA.GetLinearVelocity() + (rAperp * rigidObjectA.GetAngularVelocity()) ) 
                                            - (rigidObjectB.GetLinearVelocity() + (rBperp * rigidObjectB.GetAngularVelocity()) ); 

                const float relativeVelocityAlongNormal = pe2dMath::Dot(relativeVelocity, normal);

                if(relativeVelocityAlongNormal > 0.0f)
                {
                    continue;
                }
                
                const float rAperpNormal = pe2dMath::Dot(rAperp, normal);
                const float rBperpNormal = pe2dMath::Dot(rBperp, normal);

                const float denominator = invMassA + invMassB 
                    + (rAperpNormal * rAperpNormal) * invInertiaA
                    + (rBperpNormal * rBperpNormal) * invInertiaB;

                float j = -(1.0f + coefficientOfRestitution) * relativeVelocityAlongNormal;
                j /= denominator;
                j /= (float)contactCount;
                jList[i] = j;

                impulseList[i] = j * normal;
            }   
            for(int i = 0; i < contactCount; i++)
            {
                const Vector2 impulse = impulseList[i];
                rigidObjectA.AddLinearVelocity(impulse * invMassA);
                rigidObjectA.AddAngularVelocity(pe2dMath::Cross(rAList[i], impulse) * invInertiaA);
                rigidObjectB.AddLinearVelocity(impulse * -invMassB);
                rigidObjectB.AddAngularVelocity(pe2dMath::Cross(rBList[i], impulse) * -invInertiaB);
            }
#pragma endregion

#pragma region add frcition impulse
            for(int i = 0; i < contactCount; i++)
            {
                const Vector2 rAperp = pe2dMath::Perp(rAList[i]);
                const Vector2 rBperp = pe2dMath::Perp(rBList[i]);

                const auto angVelA = rAperp * rigidObjectA.GetAngularVelocity();
                const auto angVelB = rBperp * rigidObjectB.GetAngularVelocity();

                const Vector2 relativeVelocity = (rigidObjectA.GetLinearVelocity() + angVelA) 
                                            - (rigidObjectB.GetLinearVelocity() + angVelB); 
                
                Vector2 tangent = relativeVelocity - pe2dMath::Dot(relativeVelocity, normal) * normal;

                if(pe2dMath::NearlyEquel(tangent, {0.0f, 0.0f}, 0.0005f))
                {
                    continue;
                }
                else
                {
                    tangent = pe2dMath::Normalize(tangent);
                }

                const float rAperpTang = pe2dMath::Dot(rAperp, tangent);
                const float rBperpTang = pe2dMath::Dot(rBperp, tangent);

                const float frictionDenominator = invMassA + invMassB 
                    + (rAperpTang * rAperpTang) * invInertiaA
                    + (rBperpTang * rBperpTang) * invInertiaB;

                float jt = -pe2dMath::Dot(relativeVelocity, tangent);
                jt /= frictionDenominator;
                jt /= (float)contactCount;
                
                const float j = jList[i];
                frictionImpulseList[i] = std::abs(jt) <= (j * coefficientOfStaticFriction) ?
                        jt * tangent : -j * tangent * coefficientOfDynamicFriction;
            }
            
            for(int i = 0; i < contactCount; i++)
            {
                const Vector2 ftictionImpulse = frictionImpulseList[i];

                rigidObjectA.AddLinearVelocity(ftictionImpulse * invMassA);
                rigidObjectA.AddAngularVelocity(pe2dMath::Cross(rAList[i], ftictionImpulse) * invInertiaA);
                rigidObjectB.AddLinearVelocity(ftictionImpulse * -invMassB);
                rigidObjectB.AddAngularVelocity(pe2dMath::Cross(rBList[i], ftictionImpulse) * -invInertiaB);
            }
#pragma endregion
        }
    }
}