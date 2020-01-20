#ifndef RESOURCE_PACK_HPP
#define RESOURCE_PACK_HPP

#include <GL/glew.h>
#include <map>
#include <vector>
#include <GLFW/glfw3.h>

class ResourcePack {
public:
    ResourcePack();
    void load_shader(std::string& name);
    GLuint get_shader(std::string& name);
private:
    std::map<std::string, GLuint> shaders;
};

#endif
