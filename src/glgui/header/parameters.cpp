#include "parameters.h"
#include <external/imgui/imgui.h>

namespace glgui {
namespace header {
    Parameters::Parameters(int num_beams)
    {
        _num_beams = num_beams;
    }
    Parameters::~Parameters()
    {
    }
    void Parameters::render()
    {
        if (ImGui::CollapsingHeader("Parameters")) {
            ImGui::SliderInt("#beams", &_num_beams, 1, 1000); // Edit 1 float using a slider from 0.0f to 1.0f
        }
    }
}
}