#include "icp.h"
#include <iostream>
#include <tuple>
using std::cout;
using std::endl;

namespace slam {
namespace scan_matching {
    IterativeClosestPoints::IterativeClosestPoints(Associator* associator, double step_size, double max_num_iterations, double diminution_threshold)
    {
        _associator = associator;
        _step_size = step_size;
        _max_num_iterations = max_num_iterations;
        _diminution_threshold = diminution_threshold;
    }
    double IterativeClosestPoints::compute_mean_squared_distance(std::vector<glm::vec2>& prev_scan_points,
        std::vector<glm::vec2>& current_scan_points,
        std::vector<std::tuple<int, int>> associated_indices,
        double d0_x,
        double d0_y,
        double theta0)
    {
        double mean_distance = 0;
        for (auto& pair : associated_indices) {
            int current_scan_index = std::get<0>(pair);
            int prev_scan_index = std::get<1>(pair);
            glm::vec2& prev_scan = prev_scan_points[prev_scan_index];
            glm::vec2& current_scan = current_scan_points[current_scan_index];
            double prev_scan_x = prev_scan.x * cos(theta0) - prev_scan.y * sin(theta0) + d0_x;
            double prev_scan_y = prev_scan.x * sin(theta0) + prev_scan.y * cos(theta0) + d0_y;
            float distance = sqrt((current_scan.x - prev_scan_x) * (current_scan.x - prev_scan_x) + (current_scan.y - prev_scan_y) * (current_scan.y - prev_scan_y));
            mean_distance += distance;
        }
        return mean_distance / (double)associated_indices.size();
    }
    void IterativeClosestPoints::match(std::vector<glm::vec2>& prev_scan_points,
        std::vector<glm::vec2>& current_scan_points,
        glm::vec2& d0,
        double& theta0)
    {
        std::vector<std::tuple<int, int>> associated_indices;
        double diminution = 0;
        double prev_error = 0;
        for (int iter = 0; iter < _max_num_iterations; iter++) {
            double ratio = _associator->find_correspondence(prev_scan_points, current_scan_points, associated_indices, d0, theta0);
            double error = compute_mean_squared_distance(prev_scan_points, current_scan_points, associated_indices, d0.x, d0.y, theta0);
            double eps = 1e-4;
            double grad_x = compute_mean_squared_distance(prev_scan_points, current_scan_points, associated_indices, d0.x + eps, d0.y, theta0);
            double grad_y = compute_mean_squared_distance(prev_scan_points, current_scan_points, associated_indices, d0.x, d0.y + eps, theta0);
            double grad_theta = compute_mean_squared_distance(prev_scan_points, current_scan_points, associated_indices, d0.x, d0.y, theta0 + eps);

            d0.x -= _step_size * (grad_x - error) / eps;
            d0.y -= _step_size * (grad_y - error) / eps;
            theta0 -= _step_size * (grad_theta - error) / eps;
            // cout << "grad: " << endl;
            // cout << (grad_x - error) / eps << endl;
            // cout << (grad_y - error) / eps << endl;
            // cout << (grad_theta - error) / eps << endl;
            cout << iter << ": " << error << endl;
            if (iter > 0) {
                diminution = error - prev_error;
                if (diminution < _diminution_threshold) {
                    break;
                }
            }
            prev_error = error;
        }
    }
}
}