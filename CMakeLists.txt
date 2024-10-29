cmake_minimum_required(VERSION 3.27)

project(zero)

set(CMAKE_C_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SHADERS shaders/*.vert shaders/*.frag)
file(GLOB_RECURSE SOURCE_ASSETS assets/*)

set(GLSLC glslc)

foreach(GLSL ${SHADERS})
    get_filename_component(FILENAME ${GLSL} NAME)
    set(SPIRV ${PROJECT_BINARY_DIR}/shaders/${FILENAME}.spv)
    add_custom_command(PRE_BUILD
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND MODULES ${SPIRV})
endforeach(GLSL)

foreach(SOURCE_MODEL ${SOURCE_ASSETS})
    get_filename_component(FILENAME ${SOURCE_MODEL} NAME)
    set(TARGET_MODEL ${PROJECT_BINARY_DIR}/assets/${FILENAME})
    add_custom_command(PRE_BUILD
        OUTPUT ${TARGET_MODEL}
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_MODEL} ${TARGET_MODEL}
        DEPENDS ${SOURCE_ASSETS})
    list(APPEND TARGET_ASSETS ${TARGET_MODEL})
endforeach(SOURCE_MODEL)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SHADERS} ${MODULES} ${SOURCE_ASSETS} ${TARGET_ASSETS})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PUBLIC include/${PROJECT_NAME}.h)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})
