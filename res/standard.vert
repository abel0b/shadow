#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v_color;

uniform mat4 mvp;
uniform mat4 depth_mvp;
uniform mat4 depth_bias_mvp;

out vec3 f_color;

void main() {
    gl_Position = depth_mvp * vec4(position, 1.0);
    f_color = v_color;
}
