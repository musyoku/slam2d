#include "map.h"
#include "../core/opengl.h"
#include <glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    Map::Map()
    {
        const GLchar vertex_shader[] = R"(
#version 400
in vec2 position;
void main(void)
{
    gl_Position = vec4(position, 1.0, 1.0);
}
)";
        const GLchar fragment_shader[] = R"(
#version 400
out vec4 color;
void main(){
    color = vec4(234.0f / 255.0f, 176.0f / 255.0f, 73.0f / 255.0f, 1.0);
}
)";
        _program = opengl::create_program(vertex_shader, fragment_shader);
        _attribute_position = glGetAttribLocation(_program, "position");

        glGenBuffers(1, &_vbo);
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(0);
    }
    void Map::render(GLFWwindow* window, int x, int y, int width, int height, std::vector<GLfloat>& observed_points)
    {
        int num_points = observed_points.size() / 2;
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(x, window_height - y - height, width, height);
        glUseProgram(_program);
        glPointSize(2);
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * num_points * sizeof(GLfloat), observed_points.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(_attribute_position);
        glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_POINTS, 0, num_points);
        glBindVertexArray(0);
        glPointSize(1);
        glUseProgram(0);
    }
}
}
