cmake_minimum_required(VERSION 3.26)
project(game_of_life)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

include_directories(include)

add_executable(game_of_life
        src/main.cpp
        src/window.cpp
        src/game_of_life.cpp
)
target_link_libraries(game_of_life PRIVATE
        sfml-graphics
)
