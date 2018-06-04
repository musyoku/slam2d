#include "observer.h"
#include "../core/opengl.h"
#include <external/glm/glm.hpp>
#include <vector>

namespace glgui {
namespace view {
    Observer::Observer(glm::vec4& bg_color)
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
uniform float alpha; 
uniform vec3 bg_color; 
void main(){
    color = vec4(vec3(210.0f / 255.0f, 83.0f / 255.0f, 129.0f / 255.0f) * alpha + (1.0f - alpha) * bg_color, 1.0);
}
)";
        _program = opengl::create_program(vertex_shader, fragment_shader);
        _attribute_position = glGetAttribLocation(_program, "position");
        _bg_color = bg_color;
        glGenBuffers(1, &_vbo);
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(0);
    }
    void Observer::render(GLFWwindow* window, int x, int y, int width, int height, glm::vec2& location, std::vector<glm::vec4> scans)
    {
        int num_observation = scans.size();
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(x, window_height - y - height, width, height);
        glUseProgram(_program);
        {
            GLfloat* buffer = new GLfloat[num_observation * 4];
            for (int n = 0; n < num_observation; n++) {
                auto& value = scans[n];
                buffer[n * 4 + 0] = location.x;
                buffer[n * 4 + 1] = location.y;
                buffer[n * 4 + 2] = value.x;
                buffer[n * 4 + 3] = value.y;
            }
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, 4 * num_observation * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
            glEnableVertexAttribArray(_attribute_position);
            glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glUniform1f(glGetUniformLocation(_program, "alpha"), 0.5f);
            glUniform3f(glGetUniformLocation(_program, "bg_color"), _bg_color.x, _bg_color.y, _bg_color.z);
            glDrawArrays(GL_LINES, 0, num_observation * 2);
            glBindVertexArray(0);
            delete[] buffer;
        }
        {
            glPointSize(5);
            GLfloat* buffer = new GLfloat[num_observation * 2];
            for (int n = 0; n < num_observation; n++) {
                auto& value = scans[n];
                buffer[n * 2 + 0] = value.x;
                buffer[n * 2 + 1] = value.y;
            }
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, num_observation * sizeof(GLfloat) * 2, buffer, GL_STATIC_DRAW);
            glEnableVertexAttribArray(_attribute_position);
            glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glUniform1f(glGetUniformLocation(_program, "alpha"), 1.0f);
            glUniform3f(glGetUniformLocation(_program, "bg_color"), _bg_color.x, _bg_color.y, _bg_color.z);
            glDrawArrays(GL_POINTS, 0, num_observation);
            glBindVertexArray(0);
            glPointSize(1);
            delete[] buffer;
        }

        glUseProgram(0);
    }
}
}
