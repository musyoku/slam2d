#pragma once
#include <random>

namespace slam {
namespace sampler {
    extern std::mt19937 mt;
    double normal(double mean, double stddev);
    void set_seed(int seed);
}
}