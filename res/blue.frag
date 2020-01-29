#version 330 core

in vec3 f_normal;
in vec4 shadow_coord;

out vec4 color;

uniform sampler2D depth_map;

void main() {
    vec3 u_light = vec3(-1.0, 0.4, 0.9);
    float brightness = dot(normalize(f_normal), normalize(u_light));
    vec3 dark_color = vec3(0.64, 0.53, 0.49);
    vec3 regular_color = vec3(0.84, 0.73, 0.69);

    float visibility = 1.0;
    if (texture(depth_map, shadow_coord.xy).z < shadow_coord.z){
        visibility = 0.5;
    }

    color = visibility * vec4(mix(dark_color, regular_color, brightness), 1.0);
}
