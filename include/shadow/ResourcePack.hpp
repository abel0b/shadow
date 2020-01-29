#ifndef RESOURCE_PACK_HPP
#define RESOURCE_PACK_HPP

#include <GL/glew.h>
#include <map>
#include <vector>
#include <GLFW/glfw3.h>
#include "tiny_obj_loader.h"

struct Mesh {
    Mesh();
    Mesh(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texcoords, std::vector<float>& colors);
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<float> colors;
};

class ResourcePack {
public:
    ResourcePack();
    void load_mesh(std::string& name);
    Mesh * get_mesh(std::string& name);
    void load_shader(std::string& name);
    GLuint get_program(std::string& name);
private:
    std::map<std::string, GLuint> programs;
    std::map<std::string, Mesh> meshes;
};

#endif
