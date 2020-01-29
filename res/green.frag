#version 330 core

in vec4 shadow_coord;

out vec4 color;

uniform sampler2D depth_map;

void main() {
    float visibility = 1.0;
    if (texture(depth_map, shadow_coord.xy).r < shadow_coord.z - 0.005) {
        visibility = 0.5;
    }

    color = visibility * vec4(0.0, 0.61, 0.33, 1.0);
}
