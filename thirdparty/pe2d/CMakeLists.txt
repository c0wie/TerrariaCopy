# Physics Engine 2D
cmake_minimum_required(VERSION 3.28.0)
project(pe2d)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(pe2d STATIC)

target_sources(pe2d PRIVATE 
    src/Algo.cpp src/PhysicsWorld.cpp
    src/Grid.cpp src/RigidObject.cpp
    src/Solver.cpp)

target_include_directories(pe2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
