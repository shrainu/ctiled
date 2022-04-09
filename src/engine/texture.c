#include "texture.h"

#include <stb_image/stb_image.h>


// Teture creation & termination
Texture* engine_texture_new(const char* path, uint32_t filter) {

    // Allocate memory for the texture
    Texture* t = (Texture*) malloc(sizeof(Texture));
    
    // Load the imade
    stbi_set_flip_vertically_on_load(true);
	uint8_t* local_buffer = stbi_load(path, &t->width, &t->height, &t->bpp, 4);

    if (!local_buffer) {
        printf("ERROR : Image file \"%s\" could not be read.\n", path);

        free(t);
        return NULL;
    }

    glGenTextures(1, &t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);

    uint32_t mipmap_filter = (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, local_buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);    
    stbi_image_free(local_buffer);
    
	return t;
}

void engine_texture_free(Texture* texture) {

    glDeleteTextures(1, &texture->id);
    
    free(texture);
}

// Texture
void engine_texture_bind(Texture* texture, uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void engine_texture_unbind(Texture* texture) {
    glBindTexture(GL_TEXTURE_2D, 0);
}