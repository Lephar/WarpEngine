cmake_minimum_required(VERSION 3.27)

project(warp)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SHADERS shaders/*.vert shaders/*.frag)
file(GLOB_RECURSE SOURCE_ASSETS assets/*)

set(CONFIG_TEMPLATE ${PROJECT_SOURCE_DIR}/config.txt)
set(CONFIG_FILE ${PROJECT_BINARY_DIR}/config.txt)

add_custom_command(
    OUTPUT ${CONFIG_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CONFIG_TEMPLATE} ${CONFIG_FILE}
    DEPENDS ${CONFIG_TEMPLATE}
)

foreach(SHADER ${SHADERS})
    get_filename_component(FILENAME ${SHADER} NAME)
    set(GLSL ${PROJECT_BINARY_DIR}/shaders/${FILENAME})
    add_custom_command(
        OUTPUT ${GLSL}
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${GLSL}
        DEPENDS ${SHADER})
    list(APPEND CODES ${GLSL})
    set(SPIRV ${PROJECT_BINARY_DIR}/shaders/${FILENAME}.spv)
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER})
    list(APPEND MODULES ${SPIRV})
endforeach(SHADER)

foreach(SOURCE_MODEL ${SOURCE_ASSETS})
    get_filename_component(FILENAME ${SOURCE_MODEL} NAME)
    set(TARGET_MODEL ${PROJECT_BINARY_DIR}/assets/${FILENAME})
    add_custom_command(
        OUTPUT ${TARGET_MODEL}
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_MODEL} ${TARGET_MODEL}
        DEPENDS ${SOURCE_ASSETS})
    list(APPEND TARGET_ASSETS ${TARGET_MODEL})
endforeach(SOURCE_MODEL)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SHADERS} ${CODES} ${MODULES} ${SOURCE_ASSETS} ${TARGET_ASSETS} ${CONFIG_TEMPLATE} ${CONFIG_FILE})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PUBLIC include/pch.h)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES} shaderc_shared cglm m)
