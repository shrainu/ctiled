#include "engine/init.h"

#include "common.h"


int main() {

    // Initialize the engine
    engine_init();

    GLFWwindow* window = engine_glfw_window();
    
    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.12, 0.1, 0.22, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}