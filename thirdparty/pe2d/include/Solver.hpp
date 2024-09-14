#pragma once

#include <vector>
#include "Collision.hpp"

namespace pe2d
{   
    /*
        Function responsible for calculating collisions between rigidObjects.
        The solver only pulls them apart to prevent penetration.
    */
    void PositionSolver(std::vector<Collision> &collisions, float deltaTime);
    
    /*
        Function responsible for calculating collisions between rigidObjects.
        The solver take into account the torque, their velocity, and bounciness.
    */
    void ImpulseSolverWithoutFriction(std::vector<Collision> &collisions, float deltaTime);

    /*
        Function responsible for calculating collisions between rigidObjects.
        The solver take into account the torque, their velocity, bounciness, and both static and dynamic friction.
    */
    void ImpulseSolverWithFriction(std::vector<Collision> &collisions, float deltaTime);
}