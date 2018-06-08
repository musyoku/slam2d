#pragma once
#include "../environment/field.h"
#include <glm/glm.hpp>
#include <vector>

namespace slam {
namespace lidar {
    class Observer {
    public:
        double _noise_stddev;
        Observer(double noise_stddev);
        void observe(environment::Field* field, glm::vec2& location, float angle_rad, int num_beams, std::vector<glm::vec4>& scans);
    };
}
}