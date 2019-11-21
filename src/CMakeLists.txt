add_executable(
    3d
    main.cpp
)

target_link_libraries(
    3d
    ${OPENGL_gl_LIBRARY}
    glfw ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    imgui
)
