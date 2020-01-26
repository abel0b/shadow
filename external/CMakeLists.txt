set(OpenGL_GL_PREFERENCE "GLVND")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

add_definitions(-w)

add_library(
    imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui-backend/imgui_impl_glfw.cpp
    imgui-backend/imgui_impl_opengl3.cpp
)

target_link_libraries(
    imgui
    libglew_static
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(glew)

option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
add_subdirectory(glfw)

add_subdirectory(tinyobjloader)

include_directories(
    imgui
    glfw/include
    glew/include
)

