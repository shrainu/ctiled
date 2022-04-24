#include "game/scene/menu.h"

#include "engine/engine.h"
#include "engine/init.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/font.h"
#include "engine/renderer.h"

#include "util/common.h"


// Scene count
#define SCENE_COUNT 2


int main(int argc, char* argv[]) {

    for (int32_t i = 0; i < argc; ++i) {
        printf("ARG %d: '%s'\n", i, argv[i]);
    }

    // Initialize the engine
    if (!engine_init()) {
        printf("ERROR: Engine failed to initialize.\n");
        return -1;
    }

    // Window
    GLFWwindow* window = engine_glfw_window();

    // Load scene resources
    game_scene_menu_load();

    // Scene functions
    scene_func_t scene_functions[SCENE_COUNT] = {
        &game_scene_menu,
    };

    // Set the initial scene
    game_scene_menu_set_active();

    while(!glfwWindowShouldClose(window)) {

        // Update current scene
        for (int32_t i = 0; i < SCENE_COUNT; ++i) {
            scene_func_t func = scene_functions[i];
            if ((*func)() == SCENE_EXECUTED) {
                break;
            }
        }
    }

    // Free scene resources
    game_scene_menu_load();

    engine_terminate();

    return 0;
}
