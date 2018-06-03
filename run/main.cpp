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
    GLFWwindow* window = glfwCreateWindow(1920, 640, "slam2d", NULL, NULL);
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

    std::unique_ptr<glgui::view::Field> fieldView = std::make_unique<glgui::view::Field>(field.get());
    std::unique_ptr<glgui::view::Observer> observerView = std::make_unique<glgui::view::Observer>();

    std::unique_ptr<glgui::header::Parameters> parameters = std::make_unique<glgui::header::Parameters>(50);
    std::unique_ptr<glgui::frame::Main> mainFrame = std::make_unique<glgui::frame::Main>(parameters.get());

    int time_step = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        int screen_width, screen_height;
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int squre_length = screen_width / 3.0;
        fieldView->render(window, 0, 0, squre_length, squre_length);

        if(mainFrame->_is_slam_running){
            int num_beams = parameters->_num_beams;
            double cursor_x, cursor_y;
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            GLfloat moving_angle_rad = M_PI * time_step / 200;
            GLfloat moving_radius = 0.75;
            glm::vec2 location;
            location.x = moving_radius * cos(moving_angle_rad);
            location.y = moving_radius * sin(moving_angle_rad);
            glm::vec4* observed_values = new glm::vec4[parameters->_num_beams];
            for (int n = 0; n < num_beams; n++) {
                observed_values[n] = { 0, 0, 0, 0 };
            }
            observer->observe(field.get(), location, moving_angle_rad, num_beams, observed_values);
            for (int n = 0; n < num_beams; n++) {
                auto& value = observed_values[n];
            }

            observerView->render(window, squre_length, 0, squre_length, squre_length, location, observed_values, num_beams);
            delete[] observed_values;
        }

        mainFrame->render();

        glViewport(0, 0, screen_width, screen_height);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        time_step++;
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
