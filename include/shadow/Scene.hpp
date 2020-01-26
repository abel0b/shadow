#ifndef SCENE_HPP
#define SCENE_HPP

#include "shadow/ResourcePack.hpp"
#include "shadow/Camera.hpp"
#include <vector>

class Scene {
public:
    Scene();
    void update();
    void render(int display_w, int display_h, ResourcePack& render, Camera& camera);

private:
    int grid_width;
    int grid_height;
    int grid_start_x, grid_start_y;
    int grid_cell_size;
    GLuint grid_vertex_buffer;
    std::vector<GLfloat> grid_vertex_buffer_data;
};

#endif
