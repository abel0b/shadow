#include "shadow/App.hpp"
#include <iostream>
#include <cstdlib>
#include "imgui/imgui.h"
#include "imgui-backend/imgui_impl_glfw.h"
#include "imgui-backend/imgui_impl_opengl3.h"
#include "cimg/CImg.h"
#include <stdio.h>
#include "shadow/Console.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

using namespace cimg_library;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

App::App() :
    console(128),
    resource_pack(),
    camera(glm::vec3(0.0, 1.0, 4.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
    scene() {
    console.log("Started application");
}

int App::run() {
    CImg<unsigned char> image("../res/grass.png");

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    std::string standard_shader("standard");
    std::string depthmap_shader("depthmap");
    std::string shadow_shader("shadow");
    std::string shadowpcf_shader("shadowpcf");
    std::string blue_shader("blue");
    std::string box_mesh("box");
    resource_pack.load_shader(standard_shader);
    resource_pack.load_shader(depthmap_shader);
    resource_pack.load_shader(shadow_shader);
    resource_pack.load_shader(shadowpcf_shader);
    resource_pack.load_shader(blue_shader);
    resource_pack.load_mesh(box_mesh);

    ImVec4 clear_color = ImVec4(0.50f, 0.67f, 0.72f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    scene.update(resource_pack);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    bool settings_active = true;
    float my_color[4];

    char fps_str[256];
    char obj_str[256];

    const char * shadow_mapping_algorithm[] = {
        // "None",
        "Shadow mapping",
        "Shadow mapping PCF"
        //"Variance shadow mapping",
        //"Exponential variance shadow mapping"
    };

    Shadow shadow_types[] = {
        // Shadow::NoShadow,
        Shadow::ShadowMapping,
        Shadow::ShadowMappingPCF
    };

    const char * message = "Shadows";

    int nb_objects = 42;
    Shadow shadow = Shadow::ShadowMapping;
    double x = camera.position.x;
    double y = camera.position.y;
    double z = camera.position.z;

    auto color = ImVec4(242.0/255.0, 214.0/255.0, 75.0/255.0, 1);

    camera.initialize(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glfwSetCursorPos(window, display_w/2, display_h/2);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    bool escaping = false;

    bool light_projection = true;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(300, 480));
        ImGui::Begin("Settings", &settings_active, 0);

        ImGui::TextColored(color, "Controls");
        ImGui::Text("Move camera with arrows\nToggle camera mode with <escape>\n");

        ImGui::TextColored(color, "Shadows");
        for (int n = 0; n < IM_ARRAYSIZE(shadow_mapping_algorithm); n++) {
            if (ImGui::Selectable(shadow_mapping_algorithm[n], shadow == shadow_types[n]))
                shadow = shadow_types[n];
        }
        ImGui::Separator();

        ImGui::TextColored(color, "Light projection");
        if (ImGui::Selectable("Orthographic", light_projection))
            light_projection = true;
        if (ImGui::Selectable("Perspective", !light_projection))
            light_projection = false;
        ImGui::Separator();

        ImGui::TextColored(color, "Statistics");

        sprintf(fps_str, "%f", ImGui::GetIO().Framerate);
        ImGui::InputText("Framerate", fps_str, IM_ARRAYSIZE(fps_str));

        sprintf(obj_str, "%.3f", camera.position.x);
        ImGui::InputText("X-Coordinate", obj_str, IM_ARRAYSIZE(obj_str));

        sprintf(obj_str, "%.3f", camera.position.y);
        ImGui::InputText("Y-Coordinate", obj_str, IM_ARRAYSIZE(obj_str));

        sprintf(obj_str, "%.3f", camera.position.z);
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

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escaping) {
            escaping = true;
            glfwSetInputMode(window, GLFW_CURSOR, (paused)?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
            paused = !paused;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && escaping) {
            escaping = false;
        }

        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        glfwGetFramebufferSize(window, &display_w, &display_h);

        if(!paused) {
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            glfwSetCursorPos(window, display_w/2.0, display_h/2.0);
            camera.update_angle(delta_time, mouse_x, mouse_y, display_w, display_h);

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.move_up(delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
                camera.move_down(delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                camera.move_right(delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.move_left(delta_time);
            }
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        scene.render(shadow, light_projection, display_w, display_h, resource_pack, camera);

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
