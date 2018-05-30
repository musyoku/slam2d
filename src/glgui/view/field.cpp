#include "field.h"
#include <glgui/core/opengl.h>

namespace glgui {
namespace view {
    Field::Field()
    {
        // バーテックスシェーダのソースプログラム
        const GLchar vertex_shader[] = R"(
#version 400
in vec2 position;
in vec2 uv;
out vec2 vuv;
void main(void)
{
    gl_Position = vec4(position, 1.0, 1.0);
    vuv = uv;
}
)";

        // フラグメントシェーダのソースプログラム
        const GLchar fragment_shader[] = R"(
#version 400
uniform float transmit = 0.99;// light transmition coeficient <0,1>
int txrsiz = 512;     // max texture size [pixels]
uniform sampler2D texture;   // texture unit for light map
uniform vec2 t0;            // texture start point (mouse position) <0,1>
in vec2 vuv;          // texture end point, direction <0,1>
out vec4 color;
void main(){
    int i;
    vec2 t,dt;
    vec4 c0,c1;
    vec2 t00 = clamp(t0, 0.0f, 1.0f);
    dt=normalize(vuv-t00)/float(txrsiz);
    c0=vec4(1.0,1.0,1.0,1.0);   // light ray strength
    t=t00;
    if (dot(vuv-t,dt)>0.0){
        for (i=0;i<txrsiz;i++){
            if (dot(vuv - t, dt)<=0.000f){
                break;
            }
            c1=texture2D(texture,t);
            // c0.rgb = vec3(1.0f, 1.0f, 1.0f);
            // c0.rgb *= transmit;
            if(c1.a > 0.0f){
                c0.rgb = vec3(0.0f, 0.0f, 0.0f);
                break;
            }
            t+=dt;
        }
    }
    color=0.70*c0+0.30*texture2D(texture,vuv);  // render with ambient light
    // color=c0;                                 // render without ambient light
}
)";

        _program = opengl::create_program(vertex_shader, fragment_shader);

        // 図形データ
        const GLfloat position[][2] = {
            { -1.0, -1.0 },
            { 1.0, -1.0 },
            { 1.0, 1.0 },
            { -1.0, 1.0 }
        };
        const int num_position = 4;
        const GLfloat uv[] = {
            1,
            1,
            0,
            1,
            0,
            0,
            1,
            0,
        };
        const unsigned int indices[] = { 0, 1, 2, 3, 0 };

        // shader variable indices
        _position_location = glGetAttribLocation(_program, "position");
        _uv_location = glGetAttribLocation(_program, "uv");
        _texture_location = glGetUniformLocation(_program, "texture");
        _t0_location = glGetUniformLocation(_program, "t0");

        std::cout << _position_location << std::endl;
        std::cout << _uv_location << std::endl;
        std::cout << _texture_location << std::endl;
        std::cout << _t0_location << std::endl;

        // setup shader uniform
        glUniform1i(_texture_location, 0); // = use GL_TEXTURE0

        // setup vertex arrays
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        // send vertex array data
        glGenBuffers(1, &_vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
        glBufferData(GL_ARRAY_BUFFER, 32, position, GL_STATIC_DRAW);
        glVertexAttribPointer(_position_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(_position_location);

        glGenBuffers(1, &_vbo_uv);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
        glBufferData(GL_ARRAY_BUFFER, 32, uv, GL_STATIC_DRAW);
        glVertexAttribPointer(_uv_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(_uv_location);

        // and indices
        glGenBuffers(1, &_vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20, indices, GL_STATIC_DRAW);

        /* テクスチャの読み込みに使う配列 */
        GLubyte pixels[512 * 512 * 4];
        FILE* fp;

        /* テクスチャ画像の読み込み */
        if ((fp = fopen("robot.raw", "rb")) != NULL) {
            fread(pixels, 512 * 512 * 4, 1, fp);
            fclose(fp);
        } else {
            perror("robot.raw");
        }

        // ダミーテクスチャ
        // static unsigned char tex[] = {
        //     255, 255, 255, 0,     0,   0,   0, 0,   255, 255, 255 ,0,     0,   0,   0, 0,
        //     255,   0,   0, 255,     0, 255,   0, 255,     0,   0, 255 ,255,   255, 255, 255, 255,
        //     128,   0,   0, 255,     0, 128,   0, 255,     0,   0, 128 ,255,   128, 128, 128, 255,
        //     255, 255,   0, 255,   255,   0, 255, 255,     0, 255, 255 ,255,   255, 255, 255, 255,
        // };

        glGenTextures(1, &_texture_id);
        glBindTexture(GL_TEXTURE_2D, _texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, 512, 512, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);

        // setup sampler
        glGenSamplers(1, &_sampler_id);
        glSamplerParameteri(_sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    Field::~Field(){
        
    }
    void Field::render(GLFWwindow* window, int x, int y, int width, int height)
    {
        // シェーダプログラムの使用開始
        glUseProgram(_program);
        glViewport(0, 0, 500, 500);

        // 図形の描画
        // glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture_id);
        glBindSampler(0, _sampler_id);

        // vertices
        glBindVertexArray(_vbo_position);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);

        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        GLfloat location[2];
        location[0] = 1.0 - static_cast<GLfloat>(cursor_x) / 500.0;
        location[1] = static_cast<GLfloat>(cursor_y - (window_height - 500)) / 500;

        glUniform2fv(_t0_location, 1, location);

        // draw
        glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, 0);

        // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices);
        glBindVertexArray(0);

        // シェーダプログラムの使用終了
        glUseProgram(0);
    }
}
}
