#include "method.h"
#include <external/imgui/imgui.h>

namespace glgui {
namespace component {
    Method::Method()
    {
        _odometry_enabled = false;
        _scan_matching_enabled = false;
    }
    Method::~Method()
    {
    }
    void Method::render()
    {
        if (ImGui::CollapsingHeader("Method")) {
            ImGui::Checkbox("Odometry", &_odometry_enabled);
        }
    }
}
}