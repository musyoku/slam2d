#include "sampler.h"
#include <chrono>

namespace slam {
namespace sampler {
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    // int seed = 1;
    std::mt19937 mt(seed);
    void set_seed(int seed)
    {
        mt = std::mt19937(seed);
    }
    double normal(double mean, double stddev)
    {
        std::normal_distribution<double> rand(mean, stddev);
        return rand(mt);
    }
}
}