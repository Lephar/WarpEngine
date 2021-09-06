cmake_minimum_required(VERSION 3.20)
project(ZeroClient)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

add_executable(ZeroClient main.cpp)

add_library(ZeroSystem system/header.hpp system/window.cpp system/window.hpp system/system.cpp system/system.hpp)
target_precompile_headers(ZeroSystem PRIVATE system/header.hpp)
target_link_libraries(ZeroSystem xcb)

add_library(ZeroGraphics graphics/header.hpp graphics/graphics.cpp graphics/graphics.hpp graphics/instance.cpp graphics/instance.hpp graphics/renderer.cpp graphics/renderer.hpp graphics/surface.cpp graphics/surface.hpp graphics/details.cpp graphics/details.hpp graphics/internals.cpp graphics/internals.hpp graphics/device.cpp graphics/device.hpp)
target_precompile_headers(ZeroGraphics PRIVATE graphics/header.hpp)
target_link_libraries(ZeroGraphics xcb vulkan)

target_link_libraries(ZeroClient ZeroSystem ZeroGraphics)
