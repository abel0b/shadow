#ifndef APP_HPP
#define APP_HPP

#include "shadow/Console.hpp"
#include "shadow/ResourcePack.hpp"
#include "shadow/Camera.hpp"
#include "shadow/Scene.hpp"

class App {
public:
    App();
    int run();
private:
    Console console;
    ResourcePack resource_pack;
    Camera camera;
    Scene scene;
    bool paused = false;
    int display_w, display_h;
    double current_time = 0.0;
    float last_time = 0.0;
    float delta_time = 0.0; 
    double mouse_x, mouse_y;
};

#endif
