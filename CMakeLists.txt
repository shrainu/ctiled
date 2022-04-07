cmake_minimum_required(VERSION 3.2)
project(bwjam C)

# C standard
set(CMAKE_C_STANDARD 99)

# Runtime output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../product)

# Set debug
set(CMAKE_BUILD_TYPE Debug)

# GLFW
include_directories(/opt/homebrew/Cellar/glfw/3.3.6/include)
link_directories(/opt/homebrew/Cellar/glfw/3.3.6/lib)

# GLEW
include_directories(/opt/homebrew/Cellar/glew/2.2.0_1/include)
link_directories(/opt/homebrew/Cellar/glew/2.2.0_1/lib)

# Common
include_directories(src/)

# Source files
set(SOURCE_FILES

    # src
    src/main.c src/common.h

    # engine
    src/engine/init.c src/engine/init.h

    # parser
    src/parser/parser.c src/parser/parser.h

    # utils
    src/util/util.h src/util/map.c src/util/map.h
)

# Executable
add_executable(bwjam ${SOURCE_FILES})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(bwjam
    OpenGL::GL
    GLEW
    glfw
)