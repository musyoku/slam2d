#pragma once
#include "../environment/field.h"

namespace slam {
namespace lidar {
    class Ovserver {
    public:
        void observe(environment::Field* field, float location_x, float location_y, int num_ray);
    };
}
}