#include "renderer.h"

#include "window.h"
#include "shader.h"


// Statics
static uint32_t vao_, vbo_, ebo_;

// Shaders
static Shader quad_shader_;
static Shader text_shader_;

// Textures
static Texture* quad_texture_;

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

    // Load pre-build shaders
    quad_shader_ = engine_shader_new("res/shader/ui.vert", "res/shader/ui.frag");

    text_shader_ = engine_shader_new("res/shader/text.vert", "res/shader/text.frag");

    // Load pre-build textures
    quad_texture_ = engine_texture_new("res/texture/quad.png", GL_NEAREST);

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

    // Delete pre-build shaders
    engine_shader_free(quad_shader_);
    engine_shader_free(text_shader_);

    // Delete pre-build textures
    engine_texture_free(quad_texture_);
}

// Shaders
Shader engine_renderer_quad_shader() {
    return quad_shader_;
}

// Scissor Test
void engine_renderer_set_scissor_box(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (engine_window_get_retina()) {
        glScissor(x * 2, y * 2, w * 2, h * 2);
    } else {
        glScissor(x, y, w, h);
    }
}

void engine_renderer_set_scissor(bool value) {
    if (value) {
        glEnable(GL_SCISSOR_TEST);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

// Render functions
void engine_render_quad(Texture* texture, vec4 source, vec3 position, vec2 size) {

    // Get bound shader
    Shader bound = engine_bound_shader();

    // Bind the texture
    if (!texture) {
        texture = quad_texture_;
    }
    engine_texture_bind(texture, 0);

    // Source
    mat4s source_matrix;
    if (!source) {
        source_matrix = (mat4s) {
            0, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0
        };
    } else {
        source_matrix = (mat4s) {
            source[0]            , source[1]            , 0, 0,
            source[0] + source[2], source[1]            , 0, 0,
            source[0] + source[2], source[1] + source[3], 0, 0,
            source[0]            , source[1] + source[3], 0, 0
        };
    }

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
    engine_shader_mat4(bound, "u_source", source_matrix.raw);

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

    // Snap to pixels
    position[0] = roundf(position[0]);
    position[1] = roundf(position[1]);

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