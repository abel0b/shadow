include(ExternalProject)

set(OpenGL_GL_PREFERENCE "GLVND")

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

set(ASSIMP_BUILD_TESTS OFF)

add_library(
    imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui-backend/imgui_impl_glfw.cpp
    imgui-backend/imgui_impl_opengl3.cpp
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(assimp)

include_directories(
    imgui
    glfw/include
    glew/include
)
