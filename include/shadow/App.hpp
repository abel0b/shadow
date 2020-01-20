#ifndef APP_HPP
#define APP_HPP

#include "shadow/Console.hpp"
#include "shadow/ResourcePack.hpp"

class App {
public:
    App();
    int run();
private:
    Console console;
    ResourcePack resource_pack;
};

#endif
