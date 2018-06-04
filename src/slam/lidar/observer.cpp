#include "observer.h"
#include "../math/sampler.h"
#include <cmath>
#include <iostream>
using std::endl;

namespace slam {
namespace lidar {
    float compute_vector_length(glm::vec2& vec)
    {
        return sqrt(vec.x * vec.x + vec.y * vec.y);
    }
    bool is_inside_edge(glm::vec2& p_a, glm::vec2& p_b, glm::vec2& beam)
    {
        // ベクトル同士のなす角度から判定する
        float length_a = compute_vector_length(p_a);
        float length_b = compute_vector_length(p_b);
        float length_beam = compute_vector_length(beam);
        // if (length_beam < length_a && length_beam < length_b) {
        //     return false;
        // }
        float cos_a_beam = (p_a.x * beam.x + p_a.y * beam.y) / (length_a * length_beam);
        float cos_b_beam = (p_b.x * beam.x + p_b.y * beam.y) / (length_b * length_beam);
        float cos_a_b = (p_b.x * p_a.x + p_b.y * p_a.y) / (length_b * length_a);
        float rad_a_beam = acos(cos_a_beam);
        float rad_b_beam = acos(cos_b_beam);
        float rad_a_b = acos(cos_a_b);
        if (rad_a_beam + rad_b_beam > M_PI) {
            return false;
        }
        if (rad_a_beam > rad_a_b || rad_b_beam > rad_a_b) {
            return false;
        }
        return true;
    }
    glm::vec2 compute_new_beam_arrival_point(glm::vec2& p_1, glm::vec2& p_2, glm::vec2& old_beam_arrival_point)
    {
        // 点Cは原点
        glm::vec2& p_a = p_1.y > p_2.y ? p_2 : p_1;
        glm::vec2& p_b = p_1.y > p_2.y ? p_1 : p_2;
        float edge_ac_length = compute_vector_length(p_a);
        float edge_bc_length = compute_vector_length(p_b);
        float edge_ab_length = sqrt((p_b.x - p_a.x) * (p_b.x - p_a.x) + (p_b.y - p_a.y) * (p_b.y - p_a.y));
        float cos_a = (edge_ac_length * edge_ac_length + edge_ab_length * edge_ab_length - edge_bc_length * edge_bc_length) / (2.0 * edge_ac_length * edge_ab_length);
        float beam_length = compute_vector_length(old_beam_arrival_point);
        float cos_x = (p_a.x * old_beam_arrival_point.x + p_a.y * old_beam_arrival_point.y) / (edge_ac_length * beam_length);
        float rad_y = M_PI - acos(cos_a) - acos(cos_x);
        float new_beam_length = edge_ac_length * sin(acos(cos_a)) / sin(rad_y);
        float new_beam_scale = std::min(1.0f, new_beam_length / beam_length);
        return old_beam_arrival_point * new_beam_scale;
    }
    Observer::Observer(double noise_stddev)
    {
        _noise_stddev = noise_stddev;
    }
    void Observer::observe(environment::Field* field, glm::vec2& location, float angle_rad, int num_beams, std::vector<glm::vec4>& scans)
    {
        for (int beam_index = 0; beam_index < num_beams; beam_index++) {
            // まず位置locationから角度angle_radで無限遠にビームを飛ばしたときの到達位置を考える
            // 実際はフィールドの範囲が[-1, 1]なので無限遠でなくても適当に長さ10にしてもよい
            // 時計回りにビームを飛ばすので角度はマイナス
            // 座標系は現在の位置が原点
            float beam_angle_rad = -M_PI * 2.0 * beam_index / num_beams - M_PI * 0.1 + angle_rad;
            glm::vec2 beam_arrival_point = glm::vec2(cos(beam_angle_rad) * 10, sin(beam_angle_rad) * 10);

            // すべての壁について衝突を計算
            for (auto& wall : field->_walls) {
                // すべての三角形について
                for (int t = 0; t < wall.size() - 2; t++) {
                    // すべての辺について
                    // 座標系は現在の位置が原点
                    glm::vec2 p_a = wall[0 + t] - location;
                    glm::vec2 p_b = wall[1 + t] - location;
                    glm::vec2 p_c = wall[2 + t] - location;
                    if (is_inside_edge(p_a, p_b, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_a, p_b, beam_arrival_point);
                    }
                    if (is_inside_edge(p_b, p_c, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_b, p_c, beam_arrival_point);
                    }
                    if (is_inside_edge(p_c, p_a, beam_arrival_point)) {
                        beam_arrival_point = compute_new_beam_arrival_point(p_c, p_a, beam_arrival_point);
                    }
                }
            }

            // ノイズ
            if (_noise_stddev > 0) {
                beam_arrival_point.x += slam::sampler::normal(0, _noise_stddev);
                beam_arrival_point.y += slam::sampler::normal(0, _noise_stddev);
            }

            // 座標空間をもとに戻す
            scans[beam_index] = glm::vec4(
                beam_arrival_point.x + location.x, // グローバル座標
                beam_arrival_point.y + location.y, // グローバル座標
                compute_vector_length(beam_arrival_point), // ロボットのローカル座標
                beam_angle_rad - angle_rad); // ロボットのローカル座標
        }
    }
}
}