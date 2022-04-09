#version 330 core

in vec2 v_tex_coords;

uniform sampler2D u_texture;
uniform vec3 u_color;

out vec4 f_color;

void main() {
    f_color = vec4(u_color.xyz, texture(u_texture, v_tex_coords).r);
}