#pragma once
#include "../core/view.h"
#include <external/gl3w/gl3w.h>
#include <external/glfw/glfw3.h>
#include <external/glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    class Map : public View {
    public:
        GLuint _program;
        GLuint _attribute_position;
        GLuint _vao;
        GLuint _vbo;
        Map();
        ~Map();
        virtual void render(GLFWwindow* window, int x, int y, int width, int height, std::vector<GLfloat>& observed_points, int num_observation);
    };
}
}