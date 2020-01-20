#include "shadow/App.hpp"
#include <iostream>
#include <cstdlib>
#include "imgui/imgui.h"
#include "imgui-backend/imgui_impl_glfw.h"
#include "imgui-backend/imgui_impl_opengl3.h"
#include "cimg/CImg.h"
#include <stdio.h>
#include "shadow/Console.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

using namespace cimg_library;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

App::App()
    : console(128), resource_pack(), camera(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))  {
    console.log("Started application");
}

int App::run() {
    CImg<unsigned char> image("../res/grass.png");

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Shadow mapping", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != GLEW_OK;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    std::string debug_shader_name("debug");
    resource_pack.load_shader(debug_shader_name);

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    bool settings_active = true;
    float my_color[4];
    
    char fps_str[256];
    char obj_str[256];

    const char * shadow_mapping_algorithm[] = {
        "Perspective shadow mapping",
        "Variance shadow mapping",
        "Exponential variance shadow mapping"
    };

    const char * message = "Shadow mapping algorithm";
    
    int nb_objects = 42;
    int algorithm_selected = 0;
    double x = 12.0422424;
    double y = 25.72542;
    double z = 568.425376213;

    auto color = ImVec4(242.0/255.0, 214.0/255.0, 75.0/255.0, 1);
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();

        ImGui::SetNextWindowSize(ImVec2(300, 480));
        ImGui::Begin("Settings", &settings_active, 0);
         

        ImGui::TextColored(color, "Shadow mapping algorithm");
        for (int n = 0; n < IM_ARRAYSIZE(shadow_mapping_algorithm); n++) {
            if (ImGui::Selectable(shadow_mapping_algorithm[n], algorithm_selected == n))
                algorithm_selected = n;
        }
        ImGui::Separator();

        ImGui::TextColored(color, "Statistics");
        
        sprintf(fps_str, "%f", ImGui::GetIO().Framerate);
        ImGui::InputText("Framerate", fps_str, IM_ARRAYSIZE(fps_str));
        
        sprintf(obj_str, "%d", nb_objects);
        ImGui::InputText("Objects", obj_str, IM_ARRAYSIZE(obj_str));
       
        sprintf(obj_str, "%.3f", x);
        ImGui::InputText("X-Coordinate", obj_str, IM_ARRAYSIZE(obj_str));
        
        sprintf(obj_str, "%.3f", y);
        ImGui::InputText("Y-Coordinate", obj_str, IM_ARRAYSIZE(obj_str));

        sprintf(obj_str, "%.3f", z);
        ImGui::InputText("Z-Coordinate", obj_str, IM_ARRAYSIZE(obj_str));

        ImGui::Separator();

        ImGui::TextColored(color, "Console");
        ImGui::BeginChild("Scrolling");
        for (auto log = console.history.begin(); log != console.history.end(); ++log) {
            ImGui::Text("[%s] %s", (*log).formatted_date.c_str(), (*log).message.c_str());
        }
        ImGui::EndChild();
        
        ImGui::End(); 

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 mvp = camera.get_perspective() * camera.get_view();

        GLuint MatrixID = glGetUniformLocation(resource_pack.get_program(debug_shader_name), "matrix");

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glUseProgram(resource_pack.get_program(debug_shader_name));

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
