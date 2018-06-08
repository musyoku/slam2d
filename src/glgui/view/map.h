#pragma once
#include "../core/view.h"
#include <external/gl3w/gl3w.h>
#include <GLFW/glfw3.h>
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
        virtual void render(GLFWwindow* window, int x, int y, int width, int height, std::vector<GLfloat>& observed_points);
    };
}
}