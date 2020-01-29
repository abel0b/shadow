#version 330 core

layout(location = 0) in vec3 position;

out vec4 shadow_coord;

uniform mat4 mvp;
uniform mat4 depth_bias_mvp;

void main() {
    shadow_coord = depth_bias_mvp * vec4(position,1);
    gl_Position = mvp * vec4(position, 1.0);
}
