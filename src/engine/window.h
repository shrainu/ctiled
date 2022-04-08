#pragma once

#include "util/common.h"

// Definitions
#define WINDOW_MODE_WINDOWED            0
#define WINDOW_MODE_FULLSCREEN          1
#define WINDOW_MODE_WINDOWED_FULLSCREEN 2

// Init
bool engine_init_window();

void engine_terminate_window();

// Window
GLFWwindow* engine_glfw_window();

void engine_window_update_monitors();

void engine_window_set_monitor(uint32_t index);

void engine_windoe_set_maximize(bool maximize);

void engine_window_set_vsync(bool vsync);

void engine_window_set_mode(int32_t window_mode);

void engine_update_gl_viewport();

// Get
vec2s engine_window_get_size();