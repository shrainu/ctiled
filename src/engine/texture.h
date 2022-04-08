#pragma once

#include "util/common.h"


// Texture
typedef struct Texture {
    uint32_t id;
    int32_t bpp;
    int32_t width;
    int32_t height;
} Texture;

// Teture creation & termination
Texture* engine_texture_new(const char* path, uint32_t filter);

void engine_texture_free(Texture* texture);

// Texture
void engine_texture_bind(Texture* texture, uint32_t slot);

void engine_texture_unbind(Texture* texture);