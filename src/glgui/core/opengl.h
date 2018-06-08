#pragma once
#include <gl3w/gl3w.h>
#include <iostream>

namespace glgui {
namespace opengl {
    // シェーダオブジェクトのコンパイル結果を表示する
    GLboolean print_shader_info_log(GLuint shader, const char* str);
    GLuint create_program(const char* vertex_shader, const char* fragment_shader);
    GLboolean print_program_info_log(GLuint program);
}
}