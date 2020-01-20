#include "shadow/Camera.hpp"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position), target(target), up(up) {

}

glm::mat4 Camera::get_view() {
    return glm::lookAt(
        position,
        target,
        up
    );
}

glm::mat4 Camera::get_perspective() {
    return glm::perspective(
        glm::radians(45.0f),
        4.0f / 3.0f,
        0.1f,
        100.0f
    );
}


