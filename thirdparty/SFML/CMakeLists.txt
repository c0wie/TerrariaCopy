cmake_minimum_required(VERSION 3.28)
project(SFML)

include(FetchContent)
FetchContent_Declare( 
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)

FetchContent_MakeAvailable(SFML)