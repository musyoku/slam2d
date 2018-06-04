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
        double _step_size;
        int _max_num_iterations;
        double _diminution_threshold;

    public:
        IterativeClosestPoints(Associator* associator, double step_size, double max_num_iterations, double diminution_threshold);
        double compute_mean_squared_distance(std::vector<glm::vec2>& prev_scan_points,
            std::vector<glm::vec2>& current_scan_points,
            std::vector<std::tuple<int, int>> associated_indices,
            double d0_x,
            double d0_y,
            double theta0);
        void match(std::vector<glm::vec2>& prev_scan_points,
            std::vector<glm::vec2>& current_scan_points,
            glm::vec2& d0,
            double& theta0);
    };
}
}