#version 330 core

in vec4 shadow_coord;

out vec4 color;

uniform sampler2D depth_map;

void main() {
    float closestDepth = texture(depth_map, shadow_coord.xy).r;
    float currentDepth = shadow_coord.z;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depth_map, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depth_map, shadow_coord.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    color = (1.0 - 0.5*shadow) * vec4(0.0, 0.61, 0.33, 1.0);
}
