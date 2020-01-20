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
};

#endif
