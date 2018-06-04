#include "include.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

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
    GLFWwindow* window = glfwCreateWindow(1920, 800, "slam2d", NULL, NULL);
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
    std::unique_ptr<glgui::view::Trajectory> trajectoryView = std::make_unique<glgui::view::Trajectory>();

    std::unique_ptr<glgui::header::Parameters> parameters = std::make_unique<glgui::header::Parameters>(50, 0.5, 10);
    std::unique_ptr<glgui::frame::Main> mainFrame = std::make_unique<glgui::frame::Main>(parameters.get());

    int time_step = 0;
    std::vector<GLfloat> trajectory; // オドメトリによる軌跡
    std::vector<GLfloat> map; // 生成された地図
    std::vector<glm::vec4> observed_values; // レーザースキャナから得られた値

    glm::vec2 last_scan_location;

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

        if (mainFrame->_is_slam_running) {
            int num_beams = parameters->_num_beams;
            double cursor_x, cursor_y;
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            double round_base_angle_rad = M_PI / 100.0f * parameters->_speed;
            double round_angle_rad = -round_base_angle_rad * time_step + M_PI_2;
            double moving_radius = 0.75f;

            // オドメトリによる位置予測
            // 真値はlocation.xy
            if (trajectory.size() == 0) {
                double initial_location_x = moving_radius * cos(round_angle_rad);
                double initial_location_y = moving_radius * sin(round_angle_rad);
                trajectory.emplace_back(initial_location_x);
                trajectory.emplace_back(initial_location_y);
            } else {
                double prev_location_x = trajectory[trajectory.size() - 2];
                double prev_location_y = trajectory[trajectory.size() - 1];
                double delta_distance = sin(round_base_angle_rad / 2.0) * moving_radius * 2.0;
                double delta_angle = -(M_PI_2 - (M_PI - round_base_angle_rad) / 2.0);
                double delta_moving_distance_x = cos(delta_angle) * delta_distance;
                double delta_moving_distance_y = sin(delta_angle) * delta_distance;
                double transform_angle_rad = -(M_PI_2 - round_angle_rad);
                double next_location_x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                double next_location_y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;
                trajectory.emplace_back(next_location_x);
                trajectory.emplace_back(next_location_y);
            }

            // ロボットの移動
            glm::vec2 location;
            location.x = trajectory[trajectory.size() - 2];
            location.y = trajectory[trajectory.size() - 1];

            // レーザースキャナによる観測
            if (time_step % parameters->_laser_scanner_interval == 0) {
                if (observed_values.size() != parameters->_num_beams) {
                    observed_values.resize(parameters->_num_beams);
                }
                for (int n = 0; n < num_beams; n++) {
                    observed_values[n] = { 0, 0, 0, 0 };
                }
                observer->observe(field.get(), location, round_angle_rad, num_beams, observed_values);
                last_scan_location.x = location.x;
                last_scan_location.y = location.y;
                // 地図構築
            }

            observerView->render(window, squre_length, 0, squre_length, squre_length, last_scan_location, observed_values);
            trajectoryView->render(window, squre_length * 2, 0, squre_length, squre_length, trajectory);
            time_step++;
        }

        mainFrame->render();

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
