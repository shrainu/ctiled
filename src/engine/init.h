#pragma once

#include "common.h"

// Definitions
#define WINDOW_MODE_WINDOWED            0
#define WINDOW_MODE_FULLSCREEN          1
#define WINDOW_MODE_WINDOWED_FULLSCREEN 1

// Init
bool engine_init();

// Window
GLFWwindow* engine_glfw_window();