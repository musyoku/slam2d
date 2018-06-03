#include "../core/opengl.h"
#include "map.h"
#include <external/glm/glm.hpp>
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
    color = vec4(210.0f / 255.0f, 83.0f / 255.0f, 129.0f / 255.0f, 1.0);
}
)";
        _program = opengl::create_program(vertex_shader, fragment_shader);
        _attribute_position = glGetAttribLocation(_program, "position");

        // send vertex array data

        glGenBuffers(1, &_vbo);
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(0);
    }
    Map::~Map()
    {
    }
    void Map::render(GLFWwindow* window, int x, int y, int width, int height, glm::vec2& location, glm::vec4* observed_values, int num_observation)
    {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(x, window_height - y - height, width, height);
        glUseProgram(_program);
        {
            GLfloat* buffer = new GLfloat[num_observation * 4];
            for (int n = 0; n < num_observation; n++) {
                auto& value = observed_values[n];
                buffer[n * 4 + 0] = location.x;
                buffer[n * 4 + 1] = location.y;
                buffer[n * 4 + 2] = value.x;
                buffer[n * 4 + 3] = value.y;
            }
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, num_observation * sizeof(GLfloat) * 4, buffer, GL_STATIC_DRAW);
            glEnableVertexAttribArray(_attribute_position);
            glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glDrawArrays(GL_LINES, 0, num_observation * 2);
            glBindVertexArray(0);
            delete[] buffer;
        }
        {
            glPointSize(5);
            GLfloat* buffer = new GLfloat[num_observation * 2];
            for (int n = 0; n < num_observation; n++) {
                auto& value = observed_values[n];
                buffer[n * 2 + 0] = value.x;
                buffer[n * 2 + 1] = value.y;
            }
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, num_observation * sizeof(GLfloat) * 2, buffer, GL_STATIC_DRAW);
            glEnableVertexAttribArray(_attribute_position);
            glVertexAttribPointer(_attribute_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glDrawArrays(GL_POINTS, 0, num_observation);
            glBindVertexArray(0);
            glPointSize(1);
            delete[] buffer;
        }

        glUseProgram(0);
    }
}
}