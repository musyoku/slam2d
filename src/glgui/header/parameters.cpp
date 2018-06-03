#include "parameters.h"
#include <external/imgui/imgui.h>

namespace glgui {
namespace header {
    Parameters::Parameters(int num_beams, float speed)
    {
        _num_beams = num_beams;
        _speed = speed;
    }
    Parameters::~Parameters()
    {
    }
    void Parameters::render()
    {
        if (ImGui::CollapsingHeader("Parameters")) {
            ImGui::SliderInt("#beams", &_num_beams, 1, 1000);
            ImGui::SliderFloat("Speed", &_speed, 0, 1);
        }
    }
}
}