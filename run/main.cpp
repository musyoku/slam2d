#include "include.h"
#include <iostream>
#include <memory>
#include <stdio.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(61.0f / 255.0f, 57.0f / 255.0f, 90.0f / 255.0f, 1.00f);

    std::unique_ptr<slam::environment::Field> field = std::make_unique<slam::environment::Field>();
    std::unique_ptr<slam::lidar::Ovserver> observer = std::make_unique<slam::lidar::Ovserver>();

    float wall_thickness = 0.05;
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1, -1 + wall_thickness);
        wall.emplace_back(1, -1);
        wall.emplace_back(1, -1 + wall_thickness);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1 + wall_thickness, -1);
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1 + wall_thickness, 1);
        wall.emplace_back(-1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, 1 - wall_thickness);
        wall.emplace_back(-1, 1);
        wall.emplace_back(1, 1 - wall_thickness);
        wall.emplace_back(1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(1, -1);
        wall.emplace_back(1 - wall_thickness, -1);
        wall.emplace_back(1, 1);
        wall.emplace_back(1 - wall_thickness, 1);
        field->add_wall(wall);
    }

    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.2, -0.3);
        wall.emplace_back(-0.2, -0.3 + wall_thickness);
        wall.emplace_back(0.3, -0.3);
        wall.emplace_back(0.3, -0.3 + wall_thickness);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.4 + wall_thickness, -0.3);
        wall.emplace_back(-0.4, -0.3);
        wall.emplace_back(-0.3 + wall_thickness, 0.3);
        wall.emplace_back(-0.3, 0.3);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.3, 0.3 - wall_thickness);
        wall.emplace_back(-0.3, 0.3);
        wall.emplace_back(0.4, 0.3 - wall_thickness);
        wall.emplace_back(0.4, 0.3);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(0.3, -0.3);
        wall.emplace_back(0.3 - wall_thickness, -0.3);
        wall.emplace_back(0.3, 0.3);
        wall.emplace_back(0.3 - wall_thickness, 0.3);
        field->add_wall(wall);
    }

    std::unique_ptr<glgui::view::Field> field_view = std::make_unique<glgui::view::Field>(field.get());
    std::unique_ptr<glgui::view::Observer> observer_view = std::make_unique<glgui::view::Observer>();

    while (!glfwWindowShouldClose(window)) {

        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        static int num_beams = 50;
        {
            ImGui::SliderInt("#beams", &num_beams, 1, 1000); // Edit 1 float using a slider from 0.0f to 1.0f
        }

        // Rendering
        int screen_width, screen_height;
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int squre_length = screen_width / 3.0;
        field_view->render(window, 0, 0, squre_length, squre_length);


        // 観測
        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        glm::vec2 location = { (static_cast<GLfloat>(cursor_x) / squre_length * 2.0 - 1.0), -(static_cast<GLfloat>(cursor_y) / squre_length * 2.0 - 1.0) };
        glm::vec4* observed_values = new glm::vec4[num_beams];
        for (int n = 0; n < num_beams; n++) {
            observed_values[n] = { 0, 0, 0, 0 };
        }
        GLfloat angle_rad = 0;
        observer->observe(field.get(), location, angle_rad, num_beams, observed_values);
        for (int n = 0; n < num_beams; n++) {
            auto& value = observed_values[n];
        }

        observer_view->render(window, squre_length, 0, squre_length, squre_length, location, observed_values, num_beams);
        delete[] observed_values;

        glViewport(0, 0, screen_width, screen_height);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
