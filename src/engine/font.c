#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H


Font* engine_font_new(const char* path, uint32_t pixel_size, uint32_t filter) {

    FILE* f = NULL;
    if (!(f = fopen(path, "r"))) {
        printf("ERROR: File could not be opened to load font '%s'.\n", path);
        return NULL;
    }
    fclose(f);

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("ERROR: FreeType could not be initialized.\n");
        return NULL;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        printf("ERROR: Failed to load font '%s'.\n", path);
        return NULL;
    }
    FT_Set_Pixel_Sizes(face, 0, pixel_size); // Width calculated automatically

    // Allocate memory for the font
    Font* font = (Font*) malloc(sizeof(Font));

    // Disable bytle alignment restrictions
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; ++c) {

        // Load the character
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("WARNING: Failed to load glyph '%c'.\n", c);
            continue;
        }

        // Generate the texture
        uint32_t id;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // NOTE: Mipmap generation is removed for fonts
        //uint32_t mipmap_filter = (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;

        // NOTE: Mipmap generation is removed for fonts
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        
        // NOTE: Mipmap generation is removed for fonts
        //glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create character
        Character chr = (Character) {
            .id = id,
            .size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            .bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
            .advance = face->glyph->advance.x >> 6,
        };

        // Add character to font array
        font->characters[c] = chr;
    }

    // Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return font;
}

void engine_font_free(Font* font) {
    free(font);
}

// Get
vec2s engine_font_get_text_size(Font* font, const char* text, float scale) {

    float size_x = 0.0;
    float size_y = 0.0;

    uint32_t len = strlen(text);

    for (const char* c = text; c < text + len; ++c) {

        // Get the character from font
        const Character* chr = &font->characters[*c];

        float width  = chr->size[0] * scale;
        float height = chr->bearing[1] * scale;

        float advance = chr->advance * scale;

        if (height >= size_y) {
            size_y = height;
        }

        if (c == text + len - 1) {
            size_x += (chr->size[0] + chr->bearing[0]) * scale;
        } else {
            size_x += advance;
        }
    }

    return (vec2s) { size_x, size_y };
}