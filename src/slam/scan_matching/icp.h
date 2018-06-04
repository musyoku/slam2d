#pragma once
#include "associator.h"
#include <external/glm/glm.hpp>
#include <tuple>
#include <vector>

namespace slam {
namespace scan_matching {
    class IterativeClosestPoints {
    private:
        Associator* _associator;

    public:
        IterativeClosestPoints(Associator* associator);
    };
}
}