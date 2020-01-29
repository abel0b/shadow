#ifndef SCENE_HPP
#define SCENE_HPP

#include "shadow/ResourcePack.hpp"
#include "shadow/Camera.hpp"
#include <vector>

enum class Shadow {
    NoShadow,
    ShadowMapping,
};

class Scene {
public:
    Scene();
    void update(ResourcePack& resource_pack);
    void render(Shadow shadow, int display_w, int display_h, ResourcePack& render, Camera& camera);

private:
    int shadow_width = 1024;
    int shadow_height = 1024;
    int grid_width;
    int grid_height;
    int grid_start_x, grid_start_y;
    int grid_cell_size;
    GLuint grid_vertex_buffer;
    std::vector<GLfloat> grid_vertices;
    GLuint grid_index_buffer;
    std::vector<unsigned int> grid_indices;
    GLuint depth_map;
    GLuint object_vertex_buffer;
    GLuint object_normal_buffer;
};

#endif
