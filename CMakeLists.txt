cmake_minimum_required(VERSION 3.27)

project(Zero)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PUBLIC include/Zero.hpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
