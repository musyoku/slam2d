#include "field.h"
#include "../core/opengl.h"
#include <external/glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    Field::Field(slam::environment::Field* field)
    {
        _field = field;
        // バーテックスシェーダのソースプログラム
        const GLchar vertex_shader[] = R"(
#version 400
in vec2 position;
void main(void)
{
    gl_Position = vec4(position, 1.0, 1.0);
}
)";

        // フラグメントシェーダのソースプログラム
        const GLchar fragment_shader[] = R"(
#version 400
out vec4 color;
void main(){
    color = vec4(233.0f / 255.0f, 150.0f / 255.0f, 91.0f / 255.0f, 1.0);
}
)";

        _program = opengl::create_program(vertex_shader, fragment_shader);
        _position_location = glGetAttribLocation(_program, "position");

        unsigned int num_walls = field->_walls.size();
        _vbo = new GLuint[num_walls];
        _vao = new GLuint[num_walls];

        glGenBuffers(num_walls, _vbo);
        glGenVertexArrays(num_walls, _vao);

        for (int n = 0; n < num_walls; n++) {
            auto& wall = field->_walls[n];
            glBindVertexArray(_vao[n]);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo[n]);
            glBufferData(GL_ARRAY_BUFFER, wall.size() * sizeof(glm::vec2), wall.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(_position_location);
            glVertexAttribPointer(_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
        }

        glBindVertexArray(0);
    }
    void Field::render(GLFWwindow* window, int x, int y, int width, int height)
    {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        // シェーダプログラムの使用開始
        glUseProgram(_program);
        glViewport(x, window_height - y - height, width, height);

        // 図形の描画
        unsigned int num_walls = _field->_walls.size();
        for (int n = 0; n < num_walls; n++) {
            auto& wall = _field->_walls[n];
            glBindVertexArray(_vao[n]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, wall.size());
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}
}
