#include "renderer.h"

#include "window.h"
#include "shader.h"


// Statics
static uint32_t vao_, vbo_, ebo_;

// Shaders
static Shader text_shader_;

// Initialization & Termination
bool engine_init_renderer() {

    // Create data
    float vertex_buffer[] = {
        // Position         // Indices
        0.0, 0.0, 1.0,      0.0,
        1.0, 0.0, 1.0,      1.0,
        1.0, 1.0, 1.0,      2.0,
        0.0, 1.0, 1.0,      3.0
    };
    uint32_t vertex_size = 4 * sizeof(float);

    uint32_t index_buffer[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Initialize buffers
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), (const void*) vertex_buffer, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, vertex_size, (const void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer), (const void*) index_buffer, GL_STATIC_DRAW);

    // Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Load Pre-build
    text_shader_ = engine_shader_new("res/shader/text.vert", "res/shader/text.frag");

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void engine_terminate_renderer() {

    // Delete buffers
    glDeleteVertexArrays(1, &vao_);

    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);

    // Delete Pre-build shaders
    engine_shader_free(text_shader_);
}

// Render functions
void engine_render_quad(Texture* texture, vec3 position, vec2 size) {

    // Get bound shader
    Shader bound = engine_bound_shader();

    // Bind the texture
    engine_texture_bind(texture, 0);

    // Setup matrices
    vec2s win_size = engine_window_get_size();

    mat4 proj;
    glm_mat4_identity(proj);
    glm_ortho(0, win_size.x, 0, win_size.y, -1.0, 100.0, proj);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, position);
    glm_scale(model, (vec3){ size[0], size[1], 1.0 });
    
    mat4 mvp;
    glm_mul(proj, model, mvp);

    // Send uniforms
    engine_shader_int(bound, "u_texture", 0);
    engine_shader_mat4(bound, "u_mvp", mvp);

    // Draw the quad
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    // Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // Unbind texture
    engine_texture_unbind(texture);
}

void engine_render_text(Font* font, vec3 position, const char* text, vec3 color, float scale) {

    // Advance
    float advance = 0;

    // Bind the text shader
    engine_shader_bind(text_shader_);

    for (const char* c = text; c < text + strlen(text); ++c) {

        // Get the character from font
        const Character* chr = &font->characters[*c];

        float xpos = position[0] + advance + chr->bearing[0] * scale;
        float ypos = position[1] - (chr->size[1] - chr->bearing[1]) * scale;

        float width  = chr->size[0] * scale;
        float height = chr->size[1] * scale;

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, chr->id);

        // Setup matrices
        vec2s win_size = engine_window_get_size();

        mat4 proj;
        glm_mat4_identity(proj);
        glm_ortho(0, win_size.x, 0, win_size.y, -1.0, 100.0, proj);

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3) {xpos, ypos, position[2]});
        glm_scale(model, (vec3){ width, height, 1.0 });
        
        mat4 mvp;
        glm_mul(proj, model, mvp);

        // Send uniforms
        engine_shader_int(text_shader_, "u_texture", 0);
        engine_shader_vec3(text_shader_, "u_color", color);
        engine_shader_mat4(text_shader_, "u_mvp", mvp);

        // Draw the quad
        glBindVertexArray(vao_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        // Add to advance
        advance += chr->advance * scale;

        // Unbind buffers
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Unbind the shader
    engine_shader_unbind(0);
}