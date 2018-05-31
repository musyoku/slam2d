#include "field.h"
namespace slam {
namespace environment {
    void Field::add_wall(std::vector<glm::vec2> vertices)
    {
        _walls.push_back(vertices);
    }

    unsigned int Field::get_buffer_size()
    {
        unsigned int size = 0;
        for (auto& wall : _walls) {
            size += wall.size();
        }
        return size * sizeof(glm::vec2);
    }
}
}