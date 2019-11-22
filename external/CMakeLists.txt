include(ExternalProject)

set(OpenGL_GL_PREFERENCE "GLVND")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

add_library(
    imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_glfw.cpp
    imgui/examples/imgui_impl_opengl3.cpp
)

add_subdirectory(glew)
add_subdirectory(glfw)

include_directories(
    imgui
    glfw/include
    glew/include
)
