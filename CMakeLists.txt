cmake_minimum_required(VERSION 3.31)

set(TARGET_NAME conways_game_of_life)

project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/std:c++latest /permissive- /Zc:__cplusplus)
endif ()

find_package(SDL3 REQUIRED)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME}
        PRIVATE
        src/main.cpp
        src/GameOfLife.cpp
        src/GameOfLife.h
)

target_link_libraries(${TARGET_NAME} SDL3::SDL3)