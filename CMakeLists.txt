cmake_minimum_required(VERSION 3.14)
project(VeggenProject VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If not set, build type is Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# external library GLFW, configure and build
message(STATUS "Configuring GLFW...")

# CMake build artefacts for GLFW we do not need
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

add_subdirectory(external/glfw)

# Add GLAD
message(STATUS "Adding GLAD files...")

add_library(glad STATIC external/glad/src/glad.c)

target_include_directories(glad PUBLIC 
    external/glad/include
)

# Add ImGui
message(STATUS "Adding Dear ImGui files...")

# GLOB - Aby sme nemuseli
#     external/imgui/imgui.cpp
#     external/imgui/imgui_demo.cpp
#     external/imgui/imgui_draw.cpp... atd

file(GLOB SOURCES
    src/*.cpp
)

file(GLOB HEADERS
    src/*.h
)

file(GLOB IMGUI_SRC
    external/imgui/*.cpp
)

file(GLOB IMGUI_HEADERS
    external/imgui/*.h
)

set(IMGUI_SRC_GLFW_OGL 
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
)

set(IMGUI_HEADERS_GLFW_OGL 
    external/imgui/backends/imgui_impl_opengl3.h
    external/imgui/backends/imgui_impl_glfw.h
    external/imgui/backends/imgui_impl_opengl3_loader.h
)

# GLM 
message(STATUS "Configuring GLM...")

set(GLM_BUILD_TESTS OFF CACHE BOOL "Build the GLM tests" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the GLM tests" FORCE)

add_subdirectory(external/glm)

# OpenGL (system dependency)
find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")

file(GLOB FRAG_SHADERS src/shaders/*.frag)
file(GLOB VERT_SHADERS src/shaders/*.vert)

# VS Filters
# 1. najprv definujeme filters - source_group() nastavuje metadata pre subory
# 2. potom projekt/executable - add_executable()
source_group("Source Files\\Shaders\\Frag" FILES ${FRAG_SHADERS})
source_group("Source Files\\Shaders\\Vert" FILES ${VERT_SHADERS})

source_group("Source Files" FILES ${SOURCES})
source_group("Source Files\\ImGui" FILES ${IMGUI_SRC})
source_group("Source Files\\ImGui\\Backends" FILES ${IMGUI_SRC_GLFW_OGL})

source_group("Header Files" FILES ${HEADERS})
source_group("Header Files\\ImGui" FILES ${IMGUI_HEADERS})
source_group("Header Files\\ImGui\\Backends" FILES ${IMGUI_HEADERS_GLFW_OGL})

# Veggen application
add_executable(Veggen 
    ${SOURCES}
    ${HEADERS}
    ${FRAG_SHADERS}
    ${VERT_SHADERS}
    ${IMGUI_SRC}
    ${IMGUI_SRC_GLFW_OGL}
    ${IMGUI_HEADERS}
    ${IMGUI_HEADERS_GLFW_OGL}
)

file(GLOB CONFIGURE_DEPENDS FRAG_SHADERS
    src/shaders/*.frag
)

file(GLOB CONFIGURE_DEPENDS VERT_SHADERS
    src/shaders/*.vert
)

target_sources(Veggen PRIVATE
    ${FRAG_SHADERS}
    ${VERT_SHADERS}
)

target_compile_definitions(Veggen PRIVATE _USE_MATH_DEFINES IMGUI_DEFINE_MATH_OPERATORS)
target_precompile_headers(Veggen PRIVATE src/pch.h)

# Include directories for compiler
target_include_directories(Veggen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

# Link all external built targets
target_link_libraries(Veggen PRIVATE
    glad
    glfw
    glm::glm
    ${OPENGL_LIBRARIES}
)

if(MSVC)
    target_compile_options(Veggen PRIVATE /W4)
else()
    target_compile_options(Veggen PRIVATE -Wall -Wextra -pedantic)
endif()

# Run target (make run alebo cmake --build . --target run)
# like in makefiel  run: application
#                       ./application
add_custom_target(run
    COMMAND $<TARGET_FILE:Veggen>
    DEPENDS Veggen
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running MyApp..."
)

add_custom_command(TARGET Veggen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/shaders
            ${CMAKE_BINARY_DIR}/
)

# Project generation info
message(STATUS "==============================================")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "==============================================")