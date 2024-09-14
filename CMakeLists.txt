cmake_minimum_required(VERSION 3.28)
project(TerrariaCopy)

include(FetchContent)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)


FetchContent_Declare( 
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(SFML)

add_subdirectory(thirdparty/ImGui-SFML)
add_subdirectory(thirdparty/pe2d)

add_executable(${PROJECT_NAME} 
    main.cpp
    Map.cpp
    Tile.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics ImGui-SFML pe2d)

