#pragma once

#include "util/common.h"

#include "texture.h"
#include "font.h"


// Initialization & Termination
bool engine_init_renderer();

void engine_terminate_renderer();

// Render functions
void engine_render_quad(Texture* texture, vec3 position, vec2 size);

void engine_render_text(Font* font, vec3 position, const char* text, vec3 color, float scale);