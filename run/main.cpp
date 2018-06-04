#include "include.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void build_field(slam::environment::Field* field)
{
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
    glm::vec4 bg_color = glm::vec4(61.0f / 255.0f, 57.0f / 255.0f, 90.0f / 255.0f, 1.00f);

    std::unique_ptr<slam::environment::Field> field = std::make_unique<slam::environment::Field>();
    build_field(field.get());

    std::unique_ptr<glgui::view::Field> fieldView = std::make_unique<glgui::view::Field>(field.get());
    std::unique_ptr<glgui::view::Observer> observerView = std::make_unique<glgui::view::Observer>(bg_color);
    std::unique_ptr<glgui::view::Robot> robotView = std::make_unique<glgui::view::Robot>(0.1);
    std::unique_ptr<glgui::view::Trajectory> actualTrajectoryView = std::make_unique<glgui::view::Trajectory>(glm::vec3(153.0f / 255.0f, 214.0f / 255.0f, 202.0f / 255.0f));
    std::unique_ptr<glgui::view::Trajectory> estimatedTrajectoryView = std::make_unique<glgui::view::Trajectory>(glm::vec3(128.0f / 255.0f, 99.0f / 255.0f, 187.0f / 255.0f));
    std::unique_ptr<glgui::view::Map> mapView = std::make_unique<glgui::view::Map>();

    std::unique_ptr<glgui::component::Parameters> paramComponent = std::make_unique<glgui::component::Parameters>(50, 0.5, 10);
    std::unique_ptr<glgui::component::Noise> noiseComponent = std::make_unique<glgui::component::Noise>(0, 0);
    std::unique_ptr<glgui::component::Method> methodComponent = std::make_unique<glgui::component::Method>();
    std::unique_ptr<glgui::frame::Main> mainFrame = std::make_unique<glgui::frame::Main>(paramComponent.get(), methodComponent.get(), noiseComponent.get());

    std::unique_ptr<slam::lidar::Observer> observer = std::make_unique<slam::lidar::Observer>(noiseComponent->_scan_stddev);

    int time_step = 0;
    std::vector<GLfloat> actual_trajectory; // 実際の軌跡
    std::vector<GLfloat> estimated_trajectory; // 予測した軌跡
    std::vector<GLfloat> map; // 生成された地図
    std::vector<glm::vec4> scans; // レーザースキャナから得られた値

    double round_angle_rad = M_PI_2;
    double robot_rotation_noise = 0;
    glm::vec2 last_scan_location;
    bool is_first_scan = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        int screen_width, screen_height;
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int square_length = screen_width / 3.0;
        fieldView->render(window, 0, 0, square_length, square_length);

        if (mainFrame->_is_slam_running) {
            if (is_first_scan) {
                is_first_scan = false;
                time_step = 0;
                map.clear();
                actual_trajectory.clear();
                estimated_trajectory.clear();
                round_angle_rad = M_PI_2;
            }
            observer->_noise_stddev = noiseComponent->_scan_stddev;

            int num_beams = paramComponent->_num_beams;
            double cursor_x, cursor_y;
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            double round_base_angle_rad = M_PI / 100.0f * paramComponent->_speed;
            round_angle_rad -= round_base_angle_rad;
            double moving_radius = 0.75f;

            // ロボット地震が予測したロボットの現在位置
            glm::vec2 estimated_location;
            // 実際のロボットの位置
            glm::vec2 actual_location;

            // オドメトリによる位置予測
            if (actual_trajectory.size() == 0) {
                estimated_location.x = moving_radius * cos(round_angle_rad);
                estimated_location.y = moving_radius * sin(round_angle_rad);
                estimated_trajectory.emplace_back(estimated_location.x);
                estimated_trajectory.emplace_back(estimated_location.y);
                actual_location.x = estimated_location.x;
                actual_location.y = estimated_location.y;
                actual_trajectory.emplace_back(actual_location.x);
                actual_trajectory.emplace_back(actual_location.y);
            } else {
                // 微小時間ではロボットは直進しているとみなす
                double delta_distance = sin(round_base_angle_rad / 2.0) * moving_radius * 2.0;
                double delta_angle = -(M_PI_2 - (M_PI - round_base_angle_rad) / 2.0);
                double delta_moving_distance_x = cos(delta_angle) * delta_distance;
                double delta_moving_distance_y = sin(delta_angle) * delta_distance;
                double transform_angle_rad = -(M_PI_2 - round_angle_rad);

                double prev_location_x = estimated_trajectory[estimated_trajectory.size() - 2];
                double prev_location_y = estimated_trajectory[estimated_trajectory.size() - 1];
                estimated_location.x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                estimated_location.y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;

                // 実際のロボット位置にはノイズが乗る
                if (noiseComponent->_odometry_stddev > 0) {
                    double prev_location_x = actual_trajectory[actual_trajectory.size() - 2];
                    double prev_location_y = actual_trajectory[actual_trajectory.size() - 1];
                    robot_rotation_noise = M_PI * slam::sampler::normal(0, noiseComponent->_odometry_stddev);
                    double transform_angle_rad = -(M_PI_2 - round_angle_rad) + robot_rotation_noise;
                    actual_location.x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                    actual_location.y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;
                } else {
                    actual_location.x = estimated_location.x;
                    actual_location.y = estimated_location.y;
                }

                actual_trajectory.emplace_back(actual_location.x);
                actual_trajectory.emplace_back(actual_location.y);
                estimated_trajectory.emplace_back(estimated_location.x);
                estimated_trajectory.emplace_back(estimated_location.y);
            }

            // レーザースキャナによる観測
            if (time_step % paramComponent->_laser_scanner_interval == 0) {
                if (scans.size() != paramComponent->_num_beams) {
                    scans.resize(paramComponent->_num_beams);
                }
                for (int n = 0; n < num_beams; n++) {
                    scans[n] = glm::vec4(0, 0, 0, 0);
                }
                double robot_angle_rad = round_angle_rad - M_PI_2 + robot_rotation_noise;

                observer->observe(field.get(), actual_location, robot_angle_rad, num_beams, scans);
                last_scan_location.x = actual_location.x;
                last_scan_location.y = actual_location.y;

                // 地図構築
                for (int n = 0; n < num_beams; n++) {
                    // 座標変換
                    glm::vec4& scan = scans[n];
                    double distance = scan[2];
                    double angle_rad = scan[3];
                    glm::vec2 point;
                    if (methodComponent->_odometry_enabled) {
                        // 座標系を変換
                        angle_rad += robot_angle_rad;
                        // ロボットはノイズの影響がわからないので予測位置を使う
                        point.x = cos(angle_rad) * distance + estimated_location.x;
                        point.y = sin(angle_rad) * distance + estimated_location.y;
                    } else {
                        point.x = cos(angle_rad) * distance;
                        point.y = sin(angle_rad) * distance;
                    }
                    map.emplace_back(point.x);
                    map.emplace_back(point.y);
                }
            }

            time_step++;
        } else {
            is_first_scan = true;
        }

        mapView->render(window, square_length * 2, 0, square_length, square_length, map);
        observerView->render(window, square_length, 0, square_length, square_length, last_scan_location, scans);
        if (methodComponent->_odometry_enabled) {
            estimatedTrajectoryView->render(window, square_length * 2, 0, square_length, square_length, estimated_trajectory);
            actualTrajectoryView->render(window, square_length * 2, 0, square_length, square_length, actual_trajectory);
        }
        robotView->render(window, 0, 0, square_length, square_length, last_scan_location, 0);
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
