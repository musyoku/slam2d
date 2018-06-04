#pragma once
#include "../environment/field.h"
#include <external/glm/glm.hpp>
#include <vector>

namespace slam {
namespace lidar {
    class Ovserver {
    public:
        void observe(environment::Field* field, glm::vec2& location, float angle_rad, int num_beams, std::vector<glm::vec4>& scans);
    };
}
}