cmake_minimum_required(VERSION 3.27)

project(zero)

set(CMAKE_C_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SHADERS shaders/*.vert shaders/*.frag)
file(GLOB_RECURSE ASSETS assets/*)

set(GLSLC glslc)

foreach(GLSL ${SHADERS})
    get_filename_component(FILENAME ${GLSL} NAME)
    set(SPIRV ${PROJECT_BINARY_DIR}/shaders/${FILENAME}.spv)
    message(STATUS ${SPIRV})
    add_custom_command(PRE_BUILD
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND MODULES ${SPIRV})
endforeach(GLSL)

foreach(SOURCE ${ASSETS})
    get_filename_component(FILENAME ${SOURCE} NAME)
    set(DESTINATION ${PROJECT_BINARY_DIR}/assets/${FILENAME})
    message(STATUS ${DESTINATION})
    add_custom_command(PRE_BUILD
        OUTPUT ${DESTINATION}
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE} ${DESTINATION}
        DEPENDS ${ASSETS})
    list(APPEND MODELS ${DESTINATION})
endforeach(SOURCE)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SHADERS} ${MODULES} ${ASSETS} ${MODELS})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PUBLIC include/${PROJECT_NAME}.h)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})
