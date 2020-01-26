#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v_color;

uniform mat4 matrix;

out vec3 f_color;

void main() {
    gl_Position = matrix * vec4(position, 1.0);
    f_color = v_color;
}
