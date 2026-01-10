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

# ============================================================
# OpenGL (system dependency)
# ============================================================
find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")


set(SOURCES 
    src/main.cpp
)

# Add include/header.h
#set(HEADERS )

# Veggen application
add_executable(Veggen 
    ${SOURCES}
    #${HEADERS}    
)

# Include directories for compiler
target_include_directories(Veggen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
)

# Link all external built targets
target_link_libraries(Veggen PRIVATE
    glfw
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

# Project generation info
message(STATUS "==============================================")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "==============================================")