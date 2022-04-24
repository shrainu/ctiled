#include "init.h"

#include "window.h"
#include "renderer.h"
#include "input.h"


// Init
bool engine_init() {

    // Initialize the window
    if (!engine_init_window()) {
        printf("ERROR: Window creation failed!\n");
        return false;
    }

    // Initialize the renderer
    if (!engine_init_renderer()) {
        printf("ERROR: Renderer could not initialized!\n");
        return false;
    }

    // Initialize input systems
    engine_input_init_char_buffer();
    engine_input_init_key_buffer();

    return true;
}

void engine_terminate() {

    // Terminate the input system
    engine_input_free_char_buffer();
    engine_input_free_key_buffer();

    // Terminate the renderer
    engine_terminate_renderer();

    // Terminate the window
    engine_terminate_window();
}