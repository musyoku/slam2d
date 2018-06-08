#pragma once
#include "../core/view.h"
#include <external/gl3w/gl3w.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <slam/environment/field.h>

namespace glgui {
namespace view {
    class Field : public View {
    public:
        GLuint _program;
        GLuint _position_location;
        GLuint* _vao;
        GLuint* _vbo;
        slam::environment::Field* _field;
        Field(slam::environment::Field* field);
        virtual void render(GLFWwindow* window, int x, int y, int width, int height);
    };
}
}