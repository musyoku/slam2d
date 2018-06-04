#include "associator.h"

namespace slam {
namespace scan_matching {
    Associator::Associator(double distance_threshold)
    {
        _distance_threshold = distance_threshold;
    }
    double Associator::find_correspondence(std::vector<glm::vec2>& prev_scan_points, std::vector<glm::vec2>& current_scan_points, std::vector<std::tuple<int, int>>& associated_indices)
    {
        for (int current_scan_index = 0; current_scan_index < current_scan_points.size(); current_scan_index++) {
            glm::vec2& current_scan = current_scan_points[current_scan_index];
            float min_distance = 123456789;
            int min_index = -1;
            for (int prev_scan_index = 0; prev_scan_index < prev_scan_points.size(); prev_scan_index++) {
                glm::vec2& prev_scan = prev_scan_points[current_scan_index];
                float distance = sqrt((current_scan.x - prev_scan.x) * (current_scan.x - prev_scan.x) + (current_scan.y - prev_scan.y) * (current_scan.y - prev_scan.y));
                if (distance < min_distance && distance < _distance_threshold) {
                    min_distance = distance;
                    min_index = current_scan_index;
                }
            }
            if (min_index != -1) {
                associated_indices.emplace_back(std::make_tuple(current_scan_index, min_index));
            }
        }
        double ratio = associated_indices.size() / prev_scan_points.size();
        return ratio;
    }
}
}