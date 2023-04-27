cmake_minimum_required(VERSION 3.26)

project(Zero)

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(Zero ${HEADERS} ${SOURCES})
target_include_directories(Zero PRIVATE include ${SDL2_INCLUDE_DIRS})
target_link_libraries(Zero Vulkan::Vulkan ${SDL2_LIBRARIES})
