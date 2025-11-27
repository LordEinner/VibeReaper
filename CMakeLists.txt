cmake_minimum_required(VERSION 3.15)
project(VibeReaper VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add our library includes
include_directories(
    ${CMAKE_SOURCE_DIR}/lib/glad/include
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/src
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Local SDL2 Setup
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/lib/SDL2")
set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include")
set(SDL2_LIBRARY_DIR "${SDL2_PATH}/lib")

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIR})

# Manually link SDL2 (MinGW requires specific order: mingw32 -> SDL2main -> SDL2)
set(SDL2_LIBRARIES mingw32 SDL2main SDL2)

# Gather source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/*.h"
)

# Add GLAD source
list(APPEND SOURCES "lib/glad/src/glad.c")

# Create executable
add_executable(VibeReaper ${SOURCES})

# Link libraries
target_link_libraries(VibeReaper 
    PRIVATE 
    ${SDL2_LIBRARIES}
    OpenGL::GL
)

# Copy SDL2.dll to output directory
add_custom_command(TARGET VibeReaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/lib/SDL2/bin/SDL2.dll
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL2.dll
    COMMENT "Copying SDL2.dll to binary directory..."
)

# Copy assets to output directory
add_custom_command(TARGET VibeReaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Copying assets to binary directory..."
)
