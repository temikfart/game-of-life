cmake_minimum_required(VERSION 3.26)
project(game_of_life)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(MPI REQUIRED)

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

include_directories(include)
include_directories(${MPI_INCLUDE_PATH})

add_executable(game_of_life
        src/generation.cpp
        src/generation_saver.cpp
        src/main.cpp
        src/utils.cpp
        src/window.cpp
)
target_link_libraries(game_of_life PRIVATE
        sfml-graphics
        ${MPI_CXX_LIBRARIES}
)
