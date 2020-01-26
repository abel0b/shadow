#include "shadow/Camera.hpp"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position), target(target), up(up) {

}

void Camera::scroll(double xoffset, double yoffset) {
    FoV += yoffset * 5;
}

void Camera::initialize(GLFWwindow  * window) {
    // TODO: FIX
    //glfwSetWindowUserPointer(window, this);
    
    auto scroll_callback = [](GLFWwindow * window, double xoffset, double yoffset) {
        ((Camera *)(window))->scroll(xoffset, yoffset);
    };

    glfwSetScrollCallback(
        window,
        scroll_callback
    );
}

void Camera::update_angle(float delta_time, double mouse_x, double mouse_y, int display_w, int display_h) {
    horizontal_angle += mouse_speed * delta_time * (display_w/2.0 - mouse_x);
    vertical_angle   += mouse_speed * delta_time * (display_h/2.0 - mouse_y);
    
    direction = glm::vec3(
        cos(vertical_angle) * sin(horizontal_angle),
        sin(vertical_angle),
        cos(vertical_angle) * cos(horizontal_angle)
    );

    right = glm::vec3(
        sin(horizontal_angle - 3.15159f/2.0f),
        0,
        cos(horizontal_angle - 3.14159f/2.0f)
    );
}

void Camera::move_up(float delta_time) {
    position += direction * delta_time * speed;
}

void Camera::move_down(float delta_time) {
    position -= direction * delta_time * speed;
}

void Camera::move_left(float delta_time) {
    position -= right * delta_time * speed;
}

void Camera::move_right(float delta_time) {
    position += right * delta_time * speed;
}

glm::mat4 Camera::get_view() {
    up = glm::cross(right, direction);
    return glm::lookAt(
        position,
        position+direction,
        up
    );
}

glm::mat4 Camera::get_projection(float aspect) {
    // std::cout << FoV << std::endl;
    return glm::perspective(
        glm::radians(FoV),
        aspect,
        z_near,
        z_far
    );
}


