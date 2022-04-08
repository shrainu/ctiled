#version 330 core

in vec2 v_tex_coords;

uniform sampler2D u_texture;

out vec4 f_color;

void main() {
    f_color = texture(u_texture, v_tex_coords);
}