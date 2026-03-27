cmake_minimum_required(VERSION 3.16)
project(SpaceRacer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(StrikeEngine/StrikeEngine)

#target_compile_definitions(StrikeEngine PRIVATE STRIKE_COLLISION_DEBUG_EXPERIMENTAL)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})


add_compile_options(
    -Wall
    -Wextra
    -Wreorder
    -Wno-unknown-pragmas
    -Wno-deprecated-copy
    -Wno-unused-parameter
)

add_compile_options(
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)


target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

# Place executable inside build/SpaceRacer/
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}"
)

strike_setup(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/assets")