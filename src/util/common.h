#pragma once

// GLEW & GLFW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// CGLM
#include <cglm/struct.h>

// Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

// Commons
#include "list.h"

// Colors
#define COLOR_WHITE     (vec4) {255, 255, 255, 255}
#define COLOR_BLACK     (vec4) {0, 0, 0, 255}
#define COLOR_RED       (vec4) {255, 0, 0, 255}
#define COLOR_GREEN     (vec4) {0, 255, 0, 255}
#define COLOR_BLUE      (vec4) {0, 0, 255, 255}

// List declerations
LIST_DECLARE(char);