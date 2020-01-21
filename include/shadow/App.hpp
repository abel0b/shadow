#ifndef APP_HPP
#define APP_HPP

#include "shadow/Console.hpp"
#include "shadow/ResourcePack.hpp"
#include "shadow/Camera.hpp"

class App {
public:
    App();
    int run();
private:
    Console console;
    ResourcePack resource_pack;
    Camera camera;
    glm::vec3 position = glm::vec3(0.0, 0.0, 5.0);
    float horizontal_angle = 3.14159f;
    float vertical_angle = 0.0f;
    float initialFoV = 45.0f;
    
    float speed = 3.0f;
    float mouse_speed = 0.01f;
    double current_time = glfwGetTime();
    float last_time = current_time;
    float delta_time = 0.0; 
};

#endif
