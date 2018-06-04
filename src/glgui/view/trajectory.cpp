#include "trajectory.h"
#include "../core/opengl.h"
#include <external/glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    Trajectory::Trajectory(glm::vec3 color)
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
uniform vec3 line_color;
void main(){
    color = vec4(line_color, 1.0);
}
)";
        _program = opengl::create_program(vertex_shader, fragment_shader);
        _attribute_position = glGetAttribLocation(_program, "position");
        _color = color;

        glGenBuffers(1, &_vbo);
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(0);
    }
    Trajectory::~Trajectory()
    {
    }
    void Trajectory::render(GLFWwindow* window, int x, int y, int width, int height, std::vector<GLfloat>& trajectory)
    {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(x, window_height - y - height, width, height);
        glUseProgram(_program);
        int num_observation = trajectory.size() / 2;
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * num_observation * sizeof(GLfloat), trajectory.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(_attribute_position);
        glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glUniform3f(glGetUniformLocation(_program, "line_color"), _color.x, _color.y, _color.z);
        glDrawArrays(GL_LINE_STRIP, 0, num_observation);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}
}
