#pragma once
#include <vector>

namespace slam {
namespace environment {
    class Field {
    public:
        std::vector<std::vector<float>> _walls;
        void add_wall(std::vector<float> vertices);
    };
}
}