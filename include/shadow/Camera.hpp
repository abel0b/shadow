#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    glm::mat4 get_view();
    glm::mat4 get_perspective();
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

#endif
