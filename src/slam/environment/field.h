#pragma once
#include <vector>
#include <external/glm/glm.hpp>

namespace slam {
namespace environment {
    class Field {
    public:
        std::vector<std::vector<glm::vec2>> _walls;
        void add_wall(std::vector<glm::vec2> vertices);
        unsigned int get_buffer_size();
    };
}
}