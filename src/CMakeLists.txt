add_executable(
    shadow
    main.cpp
    App.cpp
    Console.cpp
    ResourcePack.cpp
    Camera.cpp
    Scene.cpp
)

target_link_libraries(
    shadow
    glfw
    imgui
    tinyobjloader
)
