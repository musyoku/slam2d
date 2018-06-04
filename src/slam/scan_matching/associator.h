#pragma once
#include <external/glm/glm.hpp>
#include <tuple>
#include <vector>

namespace slam {
namespace scan_matching {
    class Associator {
    public:
        Associator(double distance_threshold);
        double _distance_threshold;
        // 参照スキャンと現在スキャンの各点の対応付け
        double find_correspondence(std::vector<glm::vec2>& prev_scan_points,
            std::vector<glm::vec2>& current_scan_points,
            std::vector<std::tuple<int, int>>& associated_indices,
            glm::vec2& d0,
            double theta0);
    };
}
}