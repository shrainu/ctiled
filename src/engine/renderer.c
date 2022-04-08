#include "renderer.h"

#include "window.h"
#include "shader.h"


// Statics
static uint32_t vao_, vbo_, ebo_;

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

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void engine_terminate_renderer() {

    glDeleteVertexArrays(1, &vao_);

    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
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