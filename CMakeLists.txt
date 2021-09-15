cmake_minimum_required(VERSION 3.20)
project(ZeroClient)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

add_library(ZeroSystem system/base.hpp system/system.cpp system/system.hpp system/window.cpp system/window.hpp)
target_precompile_headers(ZeroSystem PRIVATE system/base.hpp)
target_link_libraries(ZeroSystem xcb)

add_library(ZeroGraphics graphics/base.hpp graphics/graphics.cpp graphics/graphics.hpp graphics/renderer.cpp graphics/renderer.hpp)
target_precompile_headers(ZeroGraphics PRIVATE graphics/base.hpp)
target_link_libraries(ZeroGraphics xcb vulkan)

add_library(ZeroNetwork network/base.hpp network/network.cpp network/network.hpp network/server.cpp network/server.hpp)
target_precompile_headers(ZeroNetwork PRIVATE network/base.hpp)
target_link_libraries(ZeroNetwork pthread)

add_library(ZeroGame game/base.hpp game/game.cpp game/game.hpp game/base.hpp)
target_precompile_headers(ZeroGame PRIVATE game/base.hpp)
target_link_libraries(ZeroGame uuid)

add_executable(ZeroClient main.cpp)
target_link_libraries(ZeroClient ZeroSystem ZeroGraphics ZeroNetwork ZeroGame)
