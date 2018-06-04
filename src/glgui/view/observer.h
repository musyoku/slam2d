#pragma once
#include "../core/view.h"
#include <external/gl3w/gl3w.h>
#include <external/glfw/glfw3.h>
#include <external/glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    class Observer : public View {
    public:
        GLuint _program;
        GLuint _attribute_position;
        GLuint _vao;
        GLuint _vbo;
        glm::vec4 _bg_color;
        Observer(glm::vec4& bg_color);
        virtual void render(GLFWwindow* window, int x, int y, int width, int height, glm::vec2& location, std::vector<glm::vec4> scans);
    };
}
}