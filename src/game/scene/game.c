#include "game.h"

#include "engine/engine.h"
#include "engine/window.h"
#include "engine/renderer.h"
#include "engine/font.h"
#include "engine/texture.h"


// Add scene definitions
SCENE_DEFINE(game);


// Resources
static Font* default_font_;

// Load
void game_scene_menu_load() {
    scene_id_ = game_scene_get_new_id();

    default_font_ = engine_font_new(
        "res/font/FiraMono-Regular.ttf", 
        FONT_DEFAULT_PIXEL_SIZE, 
        GL_LINEAR
    );
}

// Free
void game_scene_menu_free() {

    engine_font_free(default_font_);
}

// Main
int32_t game_scene_menu() {

    // Get the active scene
    uint32_t active_scene = game_scene_get_active_scene();

    if (active_scene != scene_id_) {
        return SCENE_SKIPPED;
    }

    // Setup
    GLFWwindow* window = engine_glfw_window();
    vec2s win_size = engine_window_get_size();

    // FPS
    char fps_buffer[32];
    double fps_timer = 3.0;
    vec2s fps_size = engine_font_get_text_size(default_font_, "0000", 0.25);

    // Loop
    while(active_scene == scene_id_ && !glfwWindowShouldClose(window)) {
        
        // Pre update calculations
        engine_calculate_delta_time();
        engine_calculate_fps();

        double delta_time = engine_delta_time();
        uint32_t fps = engine_fps();

        fps_timer += delta_time;
        if (fps_timer >= 0.5) {
            snprintf(fps_buffer, 32, "%u", fps);
            fps_timer = 0.0;
        }

        // Event
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        }

        // Update

        // Render
        glClearColor(0.12, 0.1, 0.22, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render fps
        engine_render_text(
            default_font_, 
            (vec3){5, win_size.y - 5 - fps_size.y, -1.0}, 
            fps_buffer, COLOR_WHITE, 0.25
        );

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Check the active scene
        active_scene = game_scene_get_active_scene();
    }

    return SCENE_EXECUTED;
}