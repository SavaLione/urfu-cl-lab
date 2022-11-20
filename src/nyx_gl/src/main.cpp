#include "main.h"

#include "SDL2/SDL_video.h"
#include "glm/fwd.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

// clang-format off
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION  120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.h>
#endif
// clang-format on

#include <CL/opencl.hpp>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_touch.h>
#include <cstdlib>
#include <exception>
#include <getopt.h>
#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#define BOOST_COMPUTE_CL_VERSION_1_2

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/async/future.hpp>
#include <boost/compute/command_queue.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/event.hpp>
#include <boost/compute/interop/opengl.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/program.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>

class image_representation
{
public:
    image_representation(std::size_t const &width, std::size_t const &height, std::size_t const &depth) : _width(width), _height(height), _depth(depth)
    {
        std::size_t size = width * height * depth;
        _image.resize(size, 0);
    }

    std::size_t const &width() const
    {
        return _width;
    }

    std::size_t const &height() const
    {
        return _height;
    }

    std::size_t const &depth() const
    {
        return _depth;
    }

    std::size_t size()
    {
        return _image.size();
    }

    uint8_t *data()
    {
        return _image.data();
    }

    std::vector<uint8_t> &get()
    {
        return _image;
    }

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _depth;
    std::vector<uint8_t> _image;
};

namespace compute = boost::compute;

GLuint compile_shader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)malloc(length);
        glGetShaderInfoLog(id, length, &length, message);
        std::string _error_message = "Compile shader error: ";
        _error_message += message;
        throw std::runtime_error(_error_message);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

const GLchar *vertex_shader = R"glsl(
    #version 150 core
    in vec2 position;
    in vec3 color;
    in vec2 texcoord;
    out vec3 Color;
    out vec2 Texcoord;
    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

const GLchar *fragment_shader = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texKitten;
    void main()
    {
        outColor = texture(texKitten, Texcoord);
    }
)glsl";

int main(int argc, char *argv[])
{
    /* Signal handler */
    signal(SIGINT, signal_callback);

    /* SDL 2.0*/
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        spdlog::error("Error: {}", SDL_GetError());
        return EXIT_FAILURE;
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window *window = SDL_CreateWindow("Laboratory work 2", 0, 0, 1024, 1024, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(!window)
    {
        spdlog::error("Error: {}", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if(!context)
    {
        spdlog::error("Error: {}", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Enable glew experimental, this enables some more OpenGL extensions.
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        spdlog::error("Failed to initialize GLEW");
        return EXIT_FAILURE;
    }

    /* SDL variables */
    int window_width  = 0;
    int window_height = 0;
    SDL_GetWindowSize(window, &window_width, &window_height);
    spdlog::info("Window width: {} height: {}", window_width, window_height);

    /* Some OpenGL settings */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    /* OpenGL */
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _ebo = 0;

    // Create Vertex Array Object
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &_vbo);

    GLfloat vertices[] = {
        //  Position      Color             Texcoords
        -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
        1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
        1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    glGenBuffers(1, &_ebo);

    GLuint elements[] = {0, 1, 2, 2, 3, 0};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint pos_attribute = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(pos_attribute);
    glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    GLint col_attribute = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(col_attribute);
    glVertexAttribPointer(col_attribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

    GLint tex_attribute = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(tex_attribute);
    glVertexAttribPointer(tex_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

    // Load textures
    std::array<GLuint, 1> textures;
    glGenTextures(1, textures.data());

    image_representation ir(window_width, window_height, 4);
    for(std::size_t i = 0, color = 0, rgba = 0; i < ir.get().size(); i++, color++, rgba++)
    {
        if(rgba == 4)
            rgba = 0;
        if(color == 255)
            color = 0;
        if(rgba == 3)
            ir.get().data()[i] = 255;
        if(rgba == 2)
            ir.get().data()[i] = 255;
        if(rgba == 1)
            ir.get().data()[i] = 0;
        if(rgba == 0)
            ir.get().data()[i] = 255;
    }

    spdlog::info("{} {} {} {}", ir.data()[0], ir.data()[1], ir.data()[2], ir.data()[3]);
    spdlog::info("{}", ir.size());
    spdlog::info("{}", ir.width());
    spdlog::info("{}", ir.height());
    spdlog::info("{}", ir.get().size());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ir.width(), ir.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.get().data());
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Main loop */
    bool exit = false;
    SDL_Event event;
    while(!exit)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            exit = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
                    break;
                case SDL_QUIT:
                    exit = true;
                    break;
                default:
                    break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    /* SDL */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void signal_callback(int signum)
{
    spdlog::info("Signal: {}", signum);

    switch(signum)
    {
        case 2:
            spdlog::info("Signal SIGINT. Exiting from the application");
            break;
        case 3:
            spdlog::info("Signal SIGQUIT. Exiting from the application");
            break;
        case 10:
            spdlog::error("Signal SIGBUS. Bus error");
            break;
        case 13:
            spdlog::error("Signal SIGPIPE. Write on a pipe with no one to read it");
            break;
        default:
            spdlog::warn("Unspecified signal: {}", signum);
            break;
    }
    exit(signum);
}