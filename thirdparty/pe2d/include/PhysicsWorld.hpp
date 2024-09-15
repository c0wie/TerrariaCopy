#pragma once

#include <unordered_map>
#include <memory>
#include <algorithm>

#include "Solver.hpp"
#include "Grid.hpp"
#include "Math.hpp"
#include "CollisionPoints.hpp"

namespace pe2d
{
    /*
        The PhysicsWorld class is the central component of the pe2d physics library. It manages all physics objects 
        and handles the entire physics simulation process. PhysicsWorld is designed to remind std::unordered_map, 
        providing key-value operations and iterators for easy management of physics objects.

        To use the PhysicsWorld class effectively, you must add a physics solver. The library provides two solvers, 
        but custom solvers can be implemented by creating a function wich returns void and has to parameters 
        std::vector<pe2d::Collision> &collisions and float deltaTime.

        PhysicsWorld also supports broad-phase collision detection using a spatial partitioning technique called a Grid. 
        The Grid can be activated by calling the appropriate function, and it helps to optimize collision detection
        with bigger simulations by dividing the simulation space into smaller regions.
    */
    class PhysicsWorld
    {
        using iterator = std::unordered_map<size_t, std::shared_ptr<RigidObject>>::iterator;
        using const_iterator = std::unordered_map<size_t, std::shared_ptr<RigidObject>>::const_iterator;
    public:
        PhysicsWorld() = default;
        explicit PhysicsWorld(unsigned int substeps) :
            m_Grid(),
            m_Substeps(std::clamp(substeps, 1U, 64U)),
            m_IsGridOn(false)
        {}
        PhysicsWorld(const PhysicsWorld &other) = delete;
        PhysicsWorld& operator= (const PhysicsWorld &other) = delete;
        PhysicsWorld(PhysicsWorld &&other) = default;
        PhysicsWorld& operator= (PhysicsWorld &&other) = default;
    public:
        void Step(float deltaTime);        
        void AddObject(std::shared_ptr<RigidObject> object);
        iterator RemoveObject(size_t ID);
        iterator RemoveObject (iterator object) { return m_Objects.erase(object); }
        iterator RemoveObjects (iterator firstObject, iterator lastObject) { return m_Objects.erase(firstObject, lastObject); } 
        void ClearObjects() { m_Objects.clear(); }

        void SetSolver(std::function<void(std::vector<Collision> &collisions, float deltaTime)> solver) { m_Solver = solver; }
        
        void AddGrid(Vector2 topLeftCorner, Vector2 bottomRightCorner, float cellSize);
        void RemoveGrid();
        void ResizeGrid(Vector2 topLeftCorner, Vector2 bottomRightCorner, float cellSize);
        
        void ResolveCollisions(float deltaTime);
        
        bool IsGridOn() const { return m_IsGridOn; }
        size_t Size() const { return m_Objects.size(); }
        bool Empty() const { return m_Objects.empty(); }
        iterator Begin() { return m_Objects.begin(); }
        iterator End() { return m_Objects.end(); }
        const_iterator cBegin() const { return m_Objects.cbegin(); }
        const_iterator cEnd() const { return m_Objects.cend(); }
        std::shared_ptr<RigidObject> At(unsigned int ID); 

    private:
        void FindCollisions(size_t IDA, size_t IDB, std::vector<Collision> &collisions);
        void ApplyGravity();
        void MoveObjects(float deltaTime);
    private:
        Grid m_Grid;
        std::function<void(std::vector<Collision> &collisions, float deltaTime)> m_Solver{PositionSolver};
        unsigned int m_Substeps{8U};
        bool m_IsGridOn{false};
        std::unordered_map<size_t, std::shared_ptr<RigidObject>> m_Objects;
    }; 
}