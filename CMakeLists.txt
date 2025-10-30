cmake_minimum_required(VERSION 3.27)

project(WarpEngine)

set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED)

set(COMPILE_OPTIONS -Wall -Wextra)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    list(APPEND COMPILE_OPTIONS -g -Og)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    list(APPEND COMPILE_OPTIONS -O3 -ffast-math -march=native -mtune=native)
endif()

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)

file(COPY data DESTINATION ${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${COMPILE_OPTIONS})
target_include_directories(${PROJECT_NAME} PUBLIC include external/include)
target_precompile_headers(${PROJECT_NAME} PUBLIC include/pch.h)
target_link_directories(${PROJECT_NAME} PUBLIC external/lib)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 Vulkan::Vulkan shaderc_shared ktx cglm m)
