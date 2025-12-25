cmake_minimum_required(VERSION 3.26)  # Just a random version that came to my head
project(plotter C)

find_package(SDL2 2.0 REQUIRED)

add_executable(plotter)

target_sources(plotter
PRIVATE
    plotter.c
    tinyexpr.c
)

target_link_libraries(plotter PRIVATE SDL2::SDL2main SDL2::SDL2)