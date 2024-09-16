cmake_minimum_required(VERSION 3.28)
project(TerrariaCopy)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)


add_subdirectory(thirdparty/ImGui-SFML)
add_subdirectory(thirdparty/SFML)

add_executable(${PROJECT_NAME} 
    main.cpp
    Map.cpp
    Tile.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics ImGui-SFML)

