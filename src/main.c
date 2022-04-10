#include "engine/engine.h"
#include "engine/init.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/font.h"
#include "engine/renderer.h"

#include "util/common.h"


int main() {

    // Initialize the engine
    if (!engine_init()) {
        printf("ERROR: Engine failed to initialize.\n");
        return -1;
    }

    GLFWwindow* window = engine_glfw_window();

    Shader shader = engine_shader_new(
        "res/shader/base.vert", 
        "res/shader/base.frag"
    );

    Texture* test = engine_texture_new("res/texture/test.jpeg", GL_LINEAR);

    Font* font = engine_font_new(
        "res/font/FiraMono-Regular.ttf", 
        FONT_DEFAULT_PIXEL_SIZE, 
        GL_LINEAR
    );

    vec2s win_size = engine_window_get_size();

    char fps_buffer[32];
    double fps_timer = 1.0;
    vec2s fps_size = engine_font_get_text_size(font, "FPS: 0000", 0.5);


    while(!glfwWindowShouldClose(window)) {

        engine_calculate_delta_time();
        engine_calculate_fps();

        // EVENTS
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        }

        double delta_time = engine_delta_time();
        uint32_t fps = engine_fps();

        fps_timer += delta_time;
        if (fps_timer >= 0.5) {
            sprintf(fps_buffer, "FPS: %u", fps);
            fps_timer = 0.0;
        }

        // UPDATE

        // RENDER
        glClearColor(0.12, 0.1, 0.22, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        engine_shader_bind(shader);

        engine_render_quad(test, (vec3){50, 50, -1.0}, (vec2){200, 200});

        engine_shader_unbind(shader);

        engine_render_text(
            font, 
            (vec3){5, win_size.y - 5 - fps_size.y, -1.0}, 
            fps_buffer, COLOR_WHITE, 0.5
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    engine_shader_free(shader);

    engine_texture_free(test);

    engine_font_free(font);

    engine_terminate();

    return 0;
}
