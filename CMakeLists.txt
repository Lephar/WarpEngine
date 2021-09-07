cmake_minimum_required(VERSION 3.20)
project(ZeroClient)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

add_executable(ZeroClient main.cpp)

add_library(ZeroSystem system/base.hpp system/system.cpp system/system.hpp system/window.cpp system/window.hpp)
target_precompile_headers(ZeroSystem PRIVATE system/base.hpp)
target_link_libraries(ZeroSystem xcb)

add_library(ZeroGraphics graphics/base.hpp graphics/graphics.cpp graphics/graphics.hpp)
target_precompile_headers(ZeroGraphics PRIVATE graphics/base.hpp)
target_link_libraries(ZeroGraphics xcb vulkan)

target_link_libraries(ZeroClient ZeroSystem ZeroGraphics)
