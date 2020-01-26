#include "shadow/Scene.hpp"
#include <iostream>
#include <string>

#include "imgui/imgui.h"
#include "imgui-backend/imgui_impl_glfw.h"
#include "imgui-backend/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

Scene::Scene() :
    grid_width(20),
    grid_height(20),
    grid_start_x(-10.0),
    grid_start_y(-10.0),
    grid_cell_size(1.0) {
    
}

void Scene::update() {
    grid_vertex_buffer_data.reserve(grid_height*grid_width);
    for(int i=0; i < grid_height; ++i) {
        for(int j=0; j < grid_width; ++j) {
            grid_vertex_buffer_data.push_back(grid_start_x+grid_cell_size*j);
            grid_vertex_buffer_data.push_back(0.0);
            grid_vertex_buffer_data.push_back(grid_start_y+grid_cell_size*i);
        }
    }
    glGenBuffers(1, &grid_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertex_buffer_data[0]) * grid_vertex_buffer_data.size(), grid_vertex_buffer_data.data(), GL_STATIC_DRAW);
}


void Scene::render(int display_w, int display_h, ResourcePack& resource_pack, Camera& camera) {
    std::string green_shader("green");

    glBindBuffer(GL_ARRAY_BUFFER, grid_vertex_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

 
    glm::mat4 mvp = camera.get_projection(static_cast<float>(display_w)/display_h) * camera.get_view();
    GLuint MatrixID = glGetUniformLocation(resource_pack.get_program(green_shader), "matrix");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glUseProgram(resource_pack.get_program(green_shader));
    glDrawArrays(GL_TRIANGLES, 0, grid_vertex_buffer_data.size());

    glDisableVertexAttribArray(0);
}
