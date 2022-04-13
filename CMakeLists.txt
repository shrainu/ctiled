cmake_minimum_required(VERSION 3.2)
project(bwjam C)

# C standard
set(CMAKE_C_STANDARD 99)

# Runtime output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../product)

# Set debug
set(CMAKE_BUILD_TYPE Debug)

# Set debug macro
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

# GLFW
include_directories(/opt/homebrew/Cellar/glfw/3.3.6/include)
link_directories(/opt/homebrew/Cellar/glfw/3.3.6/lib)

# GLEW
include_directories(/opt/homebrew/Cellar/glew/2.2.0_1/include)
link_directories(/opt/homebrew/Cellar/glew/2.2.0_1/lib)

# Freetype
include_directories(/opt/homebrew/Cellar/freetype/2.11.1/include/freetype2)
link_directories(/opt/homebrew/Cellar/freetype/2.11.1/lib)

# Common
include_directories(src/)

# Vendor
include_directories(src/vendor)

# STB Image files
set(STB_IMAGE_FILES

    src/vendor/stb_image/stb_image.c
)

# Source files
set(SOURCE_FILES

    # src
    src/main.c

    # scene
    src/game/scene/menu.c src/game/scene/menu.h
    src/game/scene/game.c src/game/scene/game.h

    # game
    src/game/scene.c src/game/scene.h

    # engine
    src/engine/init.c src/engine/init.h
    src/engine/font.c src/engine/font.h
    src/engine/engine.c src/engine/engine.h
    src/engine/shader.c src/engine/shader.h
    src/engine/window.c src/engine/window.h
    src/engine/texture.c src/engine/texture.h
    src/engine/renderer.c src/engine/renderer.h

    # parser
    src/parser/parser.c src/parser/parser.h

    # utils
    src/util/common.h
    src/util/util.h
    src/util/list.h
    src/util/map.c src/util/map.h
)

# Executable
add_executable(bwjam ${SOURCE_FILES} ${STB_IMAGE_FILES})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(bwjam
    OpenGL::GL
    freetype.a
    GLEW
    glfw
)