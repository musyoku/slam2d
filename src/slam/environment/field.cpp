#include "field.h"
namespace slam {
namespace environment {
    void Field::add_wall(std::vector<float> vertices)
    {
        _walls.push_back(vertices);
    }
}
}