#pragma once

#include "util/common.h"


// Types
typedef uint32_t Shader;

// Shader creation & termination
Shader engine_shader_new(const char* vertex_source, const char* fragment_source);

void engine_shader_free(Shader shader);

// Shader
void engine_shader_bind(Shader shader);

void engine_shader_unbind(Shader shader);

// Get
Shader engine_bound_shader();

// Shader
void engine_shader_int(Shader shader, const char* location, int32_t value);

void engine_shader_float(Shader shader, const char* location, float value);

void engine_shader_vec2(Shader shader, const char* location, vec2 vec2);

void engine_shader_vec3(Shader shader, const char* location, vec3 vec3);

void engine_shader_vec4(Shader shader, const char* location, vec4 vec4);

void engine_shader_mat4(Shader shader, const char* location, mat4 mat4);