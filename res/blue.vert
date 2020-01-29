#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 f_normal;
out vec4 shadow_coord;

uniform mat4 mvp;
uniform mat4 depth_bias_mvp;

void main() {
    f_normal = transpose(inverse(mat3(mvp))) * normal;
    shadow_coord = depth_bias_mvp * vec4(position,1.0);
    gl_Position = mvp * vec4(position, 1.0);
}
