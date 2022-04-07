#pragma once

#include "common.h"


// Types
typedef uint32_t Shader;

// Shader creation & termination
Shader engine_shader_new(const char* vertex_source, const char* fragment_source);

void engine_shader_terminate(Shader shader);