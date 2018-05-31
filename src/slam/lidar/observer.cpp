#include "observer.h"
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

namespace slam {
namespace lidar {
    bool is_inside_edge(glm::vec2& p_a, glm::vec2& p_b, glm::vec2& location, glm::vec2& beam_arrival_point)
    {
        float d_beam = (beam_arrival_point.y - location.y) / (beam_arrival_point.x - location.x);
        float d_a = (p_a.y - location.y) / (p_a.x - location.x);
        float d_b = (p_b.y - location.y) / (p_b.x - location.x);
        if (d_a >= d_beam && d_beam >= d_b) {
            return true;
        }
        if (d_b >= d_beam && d_beam >= d_a) {
            return true;
        }
        return false;
    }
    glm::vec2 compute_new_beam_arrival_point(glm::vec2& p_1, glm::vec2& p_2, glm::vec2& location, glm::vec2& old_beam_arrival_point)
    {
        // 点Cは原点
        glm::vec2 p_a = p_1.y > p_2.y ? p_2 : p_1;
        glm::vec2 p_b = p_1.y > p_2.y ? p_1 : p_2;
        float edge_ac_length = sqrt(p_a.x * p_a.x + p_a.y * p_a.y);
        float edge_bc_length = sqrt(p_b.x * p_b.x + p_b.y * p_b.y);
        float edge_ab_length = sqrt((p_b.x - p_a.x) * (p_b.x - p_a.x) + (p_b.y - p_a.y) * (p_b.y - p_a.y));
        float cos_a = (edge_ac_length * edge_ac_length + edge_ab_length * edge_ab_length - edge_bc_length * edge_bc_length) / (2.0 * edge_ac_length * edge_ab_length);
        float beam_length = sqrt(old_beam_arrival_point.x * old_beam_arrival_point.x + old_beam_arrival_point.y * old_beam_arrival_point.y);
        float cos_x = old_beam_arrival_point.x / beam_length - p_a.x / edge_ac_length;
        float rad_x = M_PI - acos(cos_a) - acos(cos_x);
        float new_beam_length = edge_ac_length * sin(acos(cos_a)) / sin(rad_x);
        float new_beam_scale = new_beam_length / beam_length;
        return old_beam_arrival_point * new_beam_scale;
    }
    void Ovserver::observe(environment::Field* field, glm::vec2& location, float angle_rad, int num_beams, glm::vec4* observed_values)
    {
        for (int beam_index = 0; beam_index < num_beams; beam_index++) {
            // まず位置locationから角度angle_radで無限遠にビームを飛ばしたときの到達位置を考える
            // 実際はフィールドの範囲が[-1, 1]なので無限遠でなくても適当に長さ100にしてもよい
            // 時計回りにビームを飛ばすので角度はマイナス
            float beam_angle_rad = -M_PI * 2.0 * beam_index / num_beams;
            glm::vec2 beam_arrival_point = { cos(beam_angle_rad) * 100, sin(beam_angle_rad) * 100 };
            // すべての壁について衝突を計算
            for (auto& wall : field->_walls) {
                // すべての三角形について
                for (int t = 0; t < wall.size() - 2; t++) {
                    // すべての辺について
                    glm::vec2& p_a = wall[0 + t];
                    glm::vec2& p_b = wall[1 + t];
                    glm::vec2& p_c = wall[2 + t];
                    if (is_inside_edge(p_a, p_b, location, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_a, p_b, location, beam_arrival_point);
                    }
                    if (is_inside_edge(p_b, p_c, location, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_b, p_c, location, beam_arrival_point);
                    }
                    if (is_inside_edge(p_c, p_a, location, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_c, p_a, location, beam_arrival_point);
                    }
                }
            }
            if (isnanf(beam_arrival_point.x)) {
                beam_arrival_point.x = 0;
            }
            if (isnanf(beam_arrival_point.y)) {
                beam_arrival_point.y = 0;
            }
            observed_values[beam_index] = {
                beam_arrival_point.x,
                beam_arrival_point.y,
                sqrt(beam_arrival_point.x * beam_arrival_point.x + beam_arrival_point.y * beam_arrival_point.y),
                beam_angle_rad
            };
        }
    }
}
}