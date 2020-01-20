#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

void main() {
    glPosition.xyz = vertexPosition_modelspace;
    glPosition.w = 1.0;
}
