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

        uint32_t mipmap_filter = (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_filter);
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
        glGenerateMipmap(GL_TEXTURE_2D);

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