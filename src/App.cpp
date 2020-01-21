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
#include "tiny_obj_loader.h"

using namespace cimg_library;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

App::App()
    : console(128), resource_pack(), camera(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))  {
    console.log("Started application");
}


float FoV = 45.0f;

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
    FoV += yoffset * 5;
}

int App::run() {
    CImg<unsigned char> image("../res/grass.png");

    std::string inputfile = "../res/box.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string terr;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &terr, inputfile.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!terr.empty()) {
        std::cerr << terr << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 330";
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

    std::string basic_shader_name("basic");
    resource_pack.load_shader(basic_shader_name);

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };


    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    bool settings_active = true;
    float my_color[4];
    
    char fps_str[256];
    char obj_str[256];

    const char * shadow_mapping_algorithm[] = {
        "Perspective shadow mapping",
        "Variance shadow mapping",
        "Exponential variance shadow mapping"
    };

    const char * message = "Shadow mapping algorithms";
    
    int nb_objects = 42;
    int algorithm_selected = 0;
    double x = 12.0422424;
    double y = 25.72542;
    double z = 568.425376213;

    auto color = ImVec4(242.0/255.0, 214.0/255.0, 75.0/255.0, 1);
    
    // position
    glfwSetScrollCallback(
        window,
        scroll_callback
    );   

    {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glfwSetCursorPos(window, display_w/2, display_h/2);
    }

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
        
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwSetCursorPos(window, display_w/2, display_h/2);
        
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        horizontal_angle += mouse_speed * delta_time * (display_w/2.0 - mouse_x);
        vertical_angle   += mouse_speed * delta_time * (display_h/2.0 - mouse_y);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }
        glm::vec3 direction(
            cos(vertical_angle) * sin(horizontal_angle),
            sin(vertical_angle),
            cos(vertical_angle) * cos(horizontal_angle)
        );

        glm::vec3 right = glm::vec3(
            sin(horizontal_angle - 3.14159f/2.0f),
            0,
            cos(horizontal_angle - 3.14159f/2.0f)
        );

        glm::mat4 projection = glm::perspective(glm::radians(FoV), (float)display_w / (float)display_h, 0.1f, 100.0f);
        glm::vec3 up = glm::cross(right, direction);

        glm::mat4 view = glm::lookAt(
            position,
            position+direction,
            up
        );

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
                position += direction * delta_time * speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
            position -= direction * delta_time * speed;
        }
        
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
            position += right * delta_time * speed;
        }
        
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
            position -= right * delta_time * speed;
        }

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 mvp = projection * view;
        // glm::mat4 mvp = camera.get_perspective() * camera.get_view();

        GLuint MatrixID = glGetUniformLocation(resource_pack.get_program(basic_shader_name), "matrix");

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glUseProgram(resource_pack.get_program(basic_shader_name));

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
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
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
