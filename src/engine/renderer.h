#pragma once

#include "util/common.h"

#include "texture.h"
#include "shader.h"
#include "font.h"


// Initialization & Termination
bool engine_init_renderer();

void engine_terminate_renderer();

// Shaders
Shader engine_renderer_quad_shader();

// Scissor Test
void engine_renderer_set_scissor_box(int32_t x, int32_t y, int32_t w, int32_t h);

void engine_renderer_set_scissor(bool value);

// Render functions
void engine_render_quad(Texture* texture, vec4 source, vec3 position, vec2 size);

void engine_render_text(Font* font, vec3 position, const char* text, vec3 color, float scale);