cmake_minimum_required(VERSION 3.7)

project(Mr-Jack)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Mr-Jack main.c GameObject.c Map.c)
target_link_libraries(Mr-Jack -lm ${SDL2_LIBRARIES})