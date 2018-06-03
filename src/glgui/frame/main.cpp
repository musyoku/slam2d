#include "main.h"
#include <external/imgui/imgui.h>

namespace glgui {
namespace frame {
    Main::Main(header::Parameters* parameters)
    {
        _parameters = parameters;
        _is_slam_running = false;
    }
    Main::~Main()
    {
    }
    void Main::render()
    {
        if (_is_slam_running) {
            if (ImGui::Button("Stop")) {
                _is_slam_running = false;
            }
        } else {
            if (ImGui::Button("Start")) {
                _is_slam_running = true;
            }
        }
        _parameters->render();
    }
}
}