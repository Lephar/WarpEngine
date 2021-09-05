cmake_minimum_required(VERSION 3.20)
project(ZeroClient)

set(CMAKE_CXX_STANDARD 23)

add_executable(ZeroClient main.cpp)

add_library(ZeroSystem system/header.hpp system/window.cpp system/window.hpp system/system.cpp system/system.hpp)
target_precompile_headers(ZeroSystem PRIVATE system/header.hpp)
target_link_libraries(ZeroSystem xcb)

target_link_libraries(ZeroClient ZeroSystem)
