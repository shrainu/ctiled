#pragma once

#include "util/common.h"

#include "texture.h"


// Initialization & Termination
bool engine_init_renderer();

void engine_terminate_renderer();

// Render functions
void engine_render_quad(Texture* texture, vec3 position, vec2 size);