#include "engine/init.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/texture.h"
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
    Texture* bunny = engine_texture_new("res/texture/bunny.png", GL_NEAREST);
    
    while(!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.12, 0.1, 0.22, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        engine_shader_bind(shader);

        engine_render_quad(test, (vec3){50, 50, -1.0}, (vec2){200, 200});
        engine_render_quad(bunny, (vec3){225, 75, -2.0}, (vec2){200, 200});

        engine_shader_unbind(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    engine_shader_free(shader);

    engine_texture_free(test);
    engine_texture_free(bunny);

    engine_terminate();

    return 0;
}