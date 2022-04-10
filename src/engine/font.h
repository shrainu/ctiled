#pragma once

#include "util/common.h"


// Character
typedef struct Character {
    uint32_t id;
    vec2 size;
    vec2 bearing;
    uint32_t advance;
} Character;

// Font
typedef struct Font {
    Character characters[128];
} Font;

// Defines
#define FONT_DEFAULT_PIXEL_SIZE 48

// Font creation & termination
Font* engine_font_new(const char* path, uint32_t pixel_size, uint32_t filter);

void engine_font_free(Font* font);

// Get
vec2s engine_font_get_text_size(Font* font, const char* text, float scale);