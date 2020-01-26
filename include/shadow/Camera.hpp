#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "imgui/imgui.h"
#include "imgui-backend/imgui_impl_glfw.h"
#include "imgui-backend/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    void initialize(GLFWwindow * window);
    void update_angle(float delta_time, double mouse_x, double mouse_y, int display_w, int display_h);
    void scroll(double xoffset, double yoffset);
    glm::mat4 get_view();
    glm::mat4 get_projection(float aspect);
    void move_up(float delta_time);
    void move_down(float delta_time);
    void move_left(float delta_time);
    void move_right(float delta_time);
    glm::vec3 position;

private:
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    float horizontal_angle = glm::pi<float>();
    float vertical_angle = 0.0f;
    float initialFoV = 45.0f;    
    float FoV = initialFoV;
    float speed = 3.0f;
    float mouse_speed = 0.01f;
    float z_near = 0.1;
    float z_far = 100.0;
 };

#endif
