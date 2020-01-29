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

void Scene::update(ResourcePack& resource_pack) {
    grid_vertices.reserve(grid_height*grid_width);
    for(int i=0; i < grid_height+1; ++i) {
        for(int j=0; j < grid_width+1; ++j) {
            grid_vertices.push_back(grid_start_x+grid_cell_size*j);
            grid_vertices.push_back(-1.0);
            grid_vertices.push_back(grid_start_y+grid_cell_size*i);
            if ((i<grid_height) && (j<grid_height)) {
                grid_indices.push_back(i*(grid_height+1)+j);
                grid_indices.push_back(i*(grid_height+1)+j+1);
                grid_indices.push_back((i+1)*(grid_height+1)+j);
                grid_indices.push_back(i*(grid_height+1)+j+1);
                grid_indices.push_back((i+1)*(grid_height+1)+j);
                grid_indices.push_back((i+1)*(grid_height+1)+j+1);
            }
        }
    }
    glGenBuffers(1, &grid_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices[0]) * grid_vertices.size(), grid_vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &grid_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grid_indices[0])*grid_indices.size(), grid_indices.data(), GL_STATIC_DRAW); 
    glGenFramebuffers(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

    std::string box_mesh("box");
    Mesh * objmesh = resource_pack.get_mesh(box_mesh);

    std::cout << objmesh->normals.size() << std::endl;

    glGenBuffers(1, &object_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, object_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * objmesh->vertices.size(), objmesh->vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &object_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, object_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * objmesh->normals.size(), objmesh->normals.data(), GL_STATIC_DRAW);
}

void Scene::render(Shadow shadow, int display_w, int display_h, ResourcePack& resource_pack, Camera& camera) {
    std::string green_shader("green");
    std::string blue_shader("blue");
    std::string box_mesh("box");
    GLuint MatrixID;

    glm::mat4 mvp = camera.get_projection(static_cast<float>(display_w)/display_h) * camera.get_view();

    // Draw ground
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

    MatrixID = glGetUniformLocation(resource_pack.get_program(green_shader), "matrix");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glUseProgram(resource_pack.get_program(green_shader));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid_index_buffer);
    glDrawElements(GL_TRIANGLES, grid_indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    
    // Draw object
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, object_vertex_buffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, object_normal_buffer);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );
    
    MatrixID = glGetUniformLocation(resource_pack.get_program(blue_shader), "matrix");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glUseProgram(resource_pack.get_program(blue_shader));

    glDrawArrays(GL_TRIANGLES, 0, resource_pack.get_mesh(box_mesh)->vertices.size());

    glDisableVertexAttribArray(0);
}
