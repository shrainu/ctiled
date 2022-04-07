#include "engine/init.h"
#include "engine/window.h"
#include "engine/shader.h"

#include "common.h"


int main() {

    // Initialize the engine
    engine_init();

    GLFWwindow* window = engine_glfw_window();

    Shader shader = engine_shader_new(
        "res/shader/base.vert", 
        "res/shader/base.frag"
    );
    
    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.12, 0.1, 0.22, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    engine_terminate();

    engine_shader_terminate(shader);

    return 0;
}