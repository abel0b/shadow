#include "shadow/Scene.hpp"
#include <iostream>
#include <string>

#include "imgui/imgui.h"
#include "imgui-backend/imgui_impl_glfw.h"
#include "imgui-backend/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() :
    grid_width(20),
    grid_height(20),
    grid_start_x(-10.0),
    grid_start_y(-10.0),
    grid_cell_size(1.0) {

}

void Scene::update(ResourcePack& resource_pack) {
    grid_vertices.reserve((grid_height+1)*(grid_width+1));
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

    glGenFramebuffers(1, &depth_map_framebuffer);

    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error creating texture" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

const char * get_gl_err_str(GLenum err) {
    switch (err) {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
    }
}

void Scene::render(Shadow shadow, int display_w, int display_h, ResourcePack& resource_pack, Camera& camera) {
    // Generate depth map
    render_pass(RenderPass::GenerateDepthMap, shadow, display_w, display_h, resource_pack, camera);

    // Render objects
    render_pass(RenderPass::Display, shadow, display_w, display_h, resource_pack, camera);

    // Debug OpenGl errors
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "GL Error: " << get_gl_err_str(err) << std::endl;
    }
}

void Scene::render_pass(RenderPass pass, Shadow shadow, int display_w, int display_h, ResourcePack& resource_pack, Camera& camera) {
    std::string green_shader("green");
    std::string blue_shader("blue");
    std::string box_mesh("box");
    std::string shadowmap_shader("shadowmap");
    GLuint uniform = -1;
    GLuint program_id = -1;

    switch (pass) {
        case RenderPass::GenerateDepthMap:
            glViewport(0, 0, shadow_width, shadow_height);
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_framebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            break;
        case RenderPass::Display:
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
    }

    glm::mat4 mvp = camera.get_projection(static_cast<float>(display_w)/display_h) * camera.get_view();
    float near_plane = -10.0f, far_plane = 10.0f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(
        glm::vec3(-2.0f, 4.0f, -1.0f),
        glm::vec3(0.0f, 0.0f,  0.0f),
        glm::vec3(0.0f, 1.0f,  0.0f)
    );
    glm::mat4 bias(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depth_mvp = lightProjection * lightView;
    // glm::mat4 depth_mvp = mvp;
    glm::mat4 depth_bias_mvp = bias * depth_mvp;

    // Draw ground
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vertex_buffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    if (pass == RenderPass::Display) {
        glBindTexture(GL_TEXTURE_2D, depth_map);
    }

    switch (pass) {
        case RenderPass::GenerateDepthMap:
            program_id = resource_pack.get_program(shadowmap_shader);
            glUseProgram(program_id);
            uniform = glGetUniformLocation(program_id, "depth_mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &depth_mvp[0][0]);
            break;
        case RenderPass::Display:
            program_id = resource_pack.get_program(green_shader);
            glUseProgram(program_id);
            uniform = glGetUniformLocation(program_id, "mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &mvp[0][0]);
            uniform = glGetUniformLocation(program_id, "depth_bias_mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &depth_bias_mvp[0][0]);
            break;
    }

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

    if (pass == RenderPass::Display) {
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
    }

    if (pass == RenderPass::Display) {
        glBindTexture(GL_TEXTURE_2D, depth_map);
    }

    switch (pass) {
        case RenderPass::GenerateDepthMap:
            program_id = resource_pack.get_program(shadowmap_shader);
            glUseProgram(program_id);
            uniform = glGetUniformLocation(program_id, "depth_mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &depth_mvp[0][0]);
            break;
        case RenderPass::Display:
            program_id = resource_pack.get_program(blue_shader);
            glUseProgram(program_id);
            uniform = glGetUniformLocation(program_id, "mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &mvp[0][0]);
            uniform = glGetUniformLocation(program_id, "depth_bias_mvp");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &depth_bias_mvp[0][0]);
            break;
    }

    glDrawArrays(GL_TRIANGLES, 0, resource_pack.get_mesh(box_mesh)->vertices.size());

    glDisableVertexAttribArray(0);

    if (pass == RenderPass::Display) {
        glDisableVertexAttribArray(1);
    }

    switch (pass) {
        case RenderPass::GenerateDepthMap:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCullFace(GL_BACK);
            break;
        case RenderPass::Display:
            break;
    }
}
