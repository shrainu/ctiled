#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in float a_index;

uniform mat4 u_mvp;
//uniform mat4 u_source;

out vec2 v_tex_coords;

void main() {
    gl_Position = u_mvp * vec4(a_position, 1.0);

    v_tex_coords = vec2(a_position.xy);

    //int index = int(a_index);
    //v_tex_coords = vec2(u_source[v_tex_coords][0], u_source[v_tex_coords][1]);
}