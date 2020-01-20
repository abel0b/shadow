add_executable(
    shadow
    main.cpp
    App.cpp
    Console.cpp
    ResourcePack.cpp
    Camera.cpp
)

target_link_libraries(
    shadow
    ${OPENGL_gl_LIBRARY}
    glfw ${GLFW_LIBRARIES}
    libglew_static
    imgui
    tinyobjloader
)
