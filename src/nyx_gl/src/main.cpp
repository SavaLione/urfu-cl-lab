#include "main.h"

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

// clang-format off
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// clang-format on

#include <CL/opencl.hpp>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_touch.h>
#include <cstdlib>
#include <exception>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Define alignment keys */
#if defined(__GNUC__) || defined(__INTEGRITY)
    #define _ALIGNED(_x) __attribute__((aligned(_x)))
#elif defined(_WIN32) && (_MSC_VER)
    /* Alignment keys neutered on windows because MSVC can't swallow function arguments with alignment requirements		*/
    /* http://msdn.microsoft.com/en-us/library/373ak2y1%28VS.71%29.aspx                                                 */
    /* #include <crtdefs.h>                                                                                             */
    /* #define _ALIGNED(_x)          _CRT_ALIGN(_x)                                                                   	*/
    #define _ALIGNED(_x)
#else
    #warning Need to implement some method to align data here
    #define _ALIGNED(_x)
#endif

/* Define capabilities for anonymous struct members. */
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define __HAS_ANON_STRUCT__ 1
    #define __ANON_STRUCT__
#elif defined(_WIN32) && defined(_MSC_VER) && !defined(__STDC__)
    #define __HAS_ANON_STRUCT__ 1
    #define __ANON_STRUCT__
#elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define __HAS_ANON_STRUCT__ 1
    #define __ANON_STRUCT__     __extension__
#else
    #define _HAS_ANON_STRUCT__ 0
    #define _ANON_STRUCT__
#endif

/* Define alignment sizes */
#if defined(__GNUC__)
    #if __x86_64__ || __ppc64__
        #define _SYSTEM_64_BIT
    #else
        #define _SYSTEM_32_BIT
    #endif
#elif defined(_WIN32) && (_MSC_VER)
    #if _WIN64
        #define _SYSTEM_64_BIT
    #else
        #define _SYSTEM_32_BIT
    #endif
#endif

#if defined(_SYSTEM_64_BIT)
    #define __SIZE_T_SIZE__ 64
#elif defined(_SYSTEM_32_BIT)
    #define __SIZE_T_SIZE__ 32
#endif

// clang-format off
typedef union
{
    uint8_t _ALIGNED(4) s[4];
    __ANON_STRUCT__ struct{uint8_t r, g, b, a;};
    __ANON_STRUCT__ struct{uint8_t s0, s1, s2, s3;};
    __ANON_STRUCT__ struct{uint8_t x, y, z, w;};
} pixel;
// clang-format on

class image
{
public:
    enum IMAGE_TYPE
    {
        RGB,
        RGBA
    };

    image(std::string const &filename)
    {
        int image_width    = 0;
        int image_height   = 0;
        int image_channels = 0;

        unsigned char *img = stbi_load(filename.c_str(), &image_width, &image_height, &image_channels, 0);

        if(img == NULL)
        {
            std::string err = "Can't load " + filename + " image";
            throw std::runtime_error(err);
        }

        _width    = image_width;
        _height   = image_height;
        _channels = image_channels;
        _pixels   = image_width * image_height;

        if(_channels == 3)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 3)
                _image.push_back({img[fl + 0], img[fl + 1], img[fl + 2], 255});
        if(_channels == 4)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 4)
                _image.push_back({img[fl + 0], img[fl + 1], img[fl + 2], img[fl + 3]});

        stbi_image_free(img);
    }

    image(std::size_t const &width, std::size_t const &height, std::size_t const &channels, IMAGE_TYPE const &image_type, uint8_t const *image)
    {
        _width      = width;
        _height     = height;
        _channels   = channels;
        _pixels     = width * height;
        _image_type = image_type;

        if(image_type == IMAGE_TYPE::RGB)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 3)
                _image.push_back({image[fl + 0], image[fl + 1], image[fl + 2], 255});
        if(image_type == IMAGE_TYPE::RGBA)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 4)
                _image.push_back({image[fl + 0], image[fl + 1], image[fl + 2], image[fl + 3]});
    }

    image(std::size_t const &width, std::size_t const &height, std::size_t const &channels, IMAGE_TYPE const &image_type, std::vector<uint8_t> const &image)
    {
        _width      = width;
        _height     = height;
        _channels   = channels;
        _pixels     = width * height;
        _image_type = image_type;

        if(image_type == IMAGE_TYPE::RGB)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 3)
                _image.push_back({image[fl + 0], image[fl + 1], image[fl + 2], 255});
        if(image_type == IMAGE_TYPE::RGBA)
            for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 4)
                _image.push_back({image[fl + 0], image[fl + 1], image[fl + 2], image[fl + 3]});
    }

    std::size_t get_width()
    {
        return _width;
    }

    std::size_t get_height()
    {
        return _height;
    }

    std::size_t get_channels()
    {
        return _channels;
    }

    std::size_t get_pixels()
    {
        return _image.size();
    }

    std::size_t size()
    {
        return _image.size();
    }

    std::vector<uint8_t> get_rgb()
    {
        std::vector<uint8_t> ret;
        for(std::size_t i = 0; i < _image.size(); i++)
        {
            ret.push_back(_image[i].r);
            ret.push_back(_image[i].g);
            ret.push_back(_image[i].b);
        }
        return ret;
    }

    std::vector<uint8_t> get_rgba()
    {
        std::vector<uint8_t> ret;
        for(std::size_t i = 0; i < _image.size(); i++)
        {
            ret.push_back(_image[i].r);
            ret.push_back(_image[i].g);
            ret.push_back(_image[i].b);
            ret.push_back(_image[i].a);
        }
        return ret;
    }

    std::vector<pixel> const &get() const
    {
        return _image;
    }

    void *data()
    {
        return _image.data();
    }

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _channels;
    std::size_t _pixels;
    IMAGE_TYPE _image_type;
    std::vector<pixel> _image;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class gpu
{
public:
    gpu()
    {
        kernels.push_back(kernel);

        try
        {
            image img("test.png");

            cl::Platform::get(&all_platforms);

            if(all_platforms.size() == 0)
            {
                throw std::runtime_error("No OpenCL platforms found.");
            }

            default_platform = all_platforms[0];

            spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

            default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

            if(all_devices.size() == 0)
            {
                throw std::runtime_error("No OpenCL devices found.");
            }

            default_device = all_devices[0];

            spdlog::info("Using OpenCL device: {}", default_device.getInfo<CL_DEVICE_NAME>());

            context = cl::Context({default_device});

            for(auto &kern : kernels)
            {
                sources.push_back({kern.c_str(), kern.length()});
            }

            program = cl::Program(context, sources);

            try
            {
                program.build({default_device});
            }
            catch(cl::BuildError err)
            {
                spdlog::error("OpenCL build error.");
                spdlog::error("Error OpenCL building: {}", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device));
                exit(EXIT_FAILURE);
            }

            cl::CommandQueue queue(context, default_device, 0, NULL);

            cl::Kernel kernel_simple_add(program, "example");
            cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);

            std::vector<uint8_t> data_image_in(img.size() * 4);
            std::vector<uint8_t> data_image_out(img.size() * 4);

            //std::copy(img.get_rgba().begin(), img.get_rgba().end(), data_image_in);
            for(std::size_t i = 0, fl = 0; i < img.size(); i++, fl += 4)
            {
                data_image_in[fl + 0] = img.get()[i].r;
                data_image_in[fl + 1] = img.get()[i].g;
                data_image_in[fl + 2] = img.get()[i].b;
                data_image_in[fl + 3] = img.get()[i].a;
            }

            cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, img.get_width(), img.get_height(), 0, &data_image_in[0]);
            cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, img.get_width(), img.get_height(), 0, NULL);

            cl::NDRange global(img.get_width(), img.get_height());

            std::array<cl::size_type, 3> origin {0, 0, 0};
            std::array<cl::size_type, 3> region {img.get_width(), img.get_height(), 1};

            kernel_simple_add.setArg(0, img_2d_in);
            kernel_simple_add.setArg(1, img_2d_out);

            queue.enqueueNDRangeKernel(kernel_simple_add, cl::NullRange, global, cl::NullRange);
            queue.finish();

            queue.enqueueReadImage(img_2d_out, CL_TRUE, origin, region, 0, 0, &data_image_out[0]);

            // image image_out(img.get_width(), img.get_height(), img.get_channels(), image::IMAGE_TYPE::RGBA, &data_image_out[0]);
        }
        catch(cl::Error &e)
        {
            spdlog::error("OpenCL error: {}", e.what());
            spdlog::error(e.err());
        }
        catch(std::exception const &e)
        {
            spdlog::error("std::exception {}", e.what());
        }
        catch(...)
        {
            spdlog::error("Unexpected exception.");
        }
    }

private:
    std::vector<cl::Platform> all_platforms;
    cl::Platform default_platform;
    std::vector<cl::Device> all_devices;
    cl::Device default_device;
    cl::Context context;
    cl::Program::Sources sources;
    std::vector<std::string> kernels;
    cl::Program program;

    std::string kernel = {"__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;"
                          ""
                          "__kernel void example("
                          "    __read_only image2d_t img_in,"
                          "    __write_only image2d_t img_out)"
                          "{"
                          "    const int x = get_global_id(0);"
                          "    const int y = get_global_id(1);"
                          "    int2 pos = (int2)(x, y);"
                          "    uint4 pixel = read_imageui(img_in, sampler, pos);"
                          "    pixel.x += 50;"
                          "    pixel.y += 50;"
                          "    pixel.z += 50;"
                          "    write_imageui(img_out, pos, pixel);"
                          "}"
                          ""
                          ""};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// // // clang-format off
// // typedef union
// // {
// //     GLfloat _ALIGNED(12) s[3];
// //     __ANON_STRUCT__ struct{uint8_t r, g, b, a;};
// //     __ANON_STRUCT__ struct{uint8_t s0, s1, s2, s3;};
// //     __ANON_STRUCT__ struct{uint8_t x, y, z, w;};
// // } triangle;
// // // clang-format on

// // clang-format off
// typedef union
// {
//     GLfloat _ALIGNED(8) s[2];
//     __ANON_STRUCT__ struct{GLfloat s0, s1;};
//     __ANON_STRUCT__ struct{GLfloat x, y;};
// } coordinate;
// // clang-format on

// struct triangle
// {
//     coordinate s0;
//     coordinate s1;
//     coordinate s2;
// };

// // clang-format off
// typedef union
// {
//     coordinate _ALIGNED(32) s[4];
//     __ANON_STRUCT__ struct{coordinate s0, s1, s2, s3;};
//     __ANON_STRUCT__ struct{coordinate x, y, z, w;};
// } square;
// // clang-format on

// class gl_image
// {
// public:
// private:

// };

/////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string frag_shader = {""
                           "#version 150"
                           "in vec2 texture_coord_from_vshader;"
                           "out vec4 out_color;"
                           ""
                           "uniform sampler2D texture_sampler;"
                           ""
                           "void main()"
                           "{"
                           "    out_color = texture(texture_sampler, texture_coord_from_vshader);"
                           "}"
                           ""};

std::string vert_shader = {""
                           "#version 150"
                           ""
                           "in vec4 position;"
                           "in vec2 texture_coord;"
                           "out vec2 texture_coord_from_vshader;"
                           ""
                           "void main()"
                           "{"
                           "    gl_Position = position;"
                           "    texture_coord_from_vshader = texture_coord;"
                           "}"
                           ""};

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

// Render scene
void display(GLuint &vao)
{
    // glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // // Swap front and back buffers
    // glfwSwapBuffers();
}

void load_image(std::string const &img)
{
    image image_from_file(img);

    // glTexImage2D(
    //     GL_TEXTURE_2D,
    //     0,
    //     GL_RGB,
    //     image_from_file.get_width(),
    //     image_from_file.get_height(),
    //     0,
    //     GL_BGR,
    //     GL_UNSIGNED_BYTE,
    //     (GLvoid *)image_from_file.data());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_from_file.get_width(), image_from_file.get_height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid *)image_from_file.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// void initialize(GLuint &vao)
// {
//     // Use a Vertex Array Object
//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);

//     // 1 square (made by 2 triangles) to be rendered
//     GLfloat vertices_position[8] = {
//         -0.5,
//         -0.5,
//         0.5,
//         -0.5,
//         0.5,
//         0.5,
//         -0.5,
//         0.5,
//     };

//     GLfloat texture_coord[8] = {
//         0.0,
//         0.0,
//         1.0,
//         0.0,
//         1.0,
//         1.0,
//         0.0,
//         1.0,
//     };

//     GLuint indices[6] = {0, 1, 2, 2, 3, 0};

//     // Create a Vector Buffer Object that will store the vertices on video memory
//     GLuint vbo;
//     glGenBuffers(1, &vbo);

//     // Allocate space for vertex positions and texture coordinates
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position) + sizeof(texture_coord), NULL, GL_STATIC_DRAW);

//     // Transfer the vertex positions:
//     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_position), vertices_position);

//     // Transfer the texture coordinates:
//     glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_position), sizeof(texture_coord), texture_coord);

//     // Create an Element Array Buffer that will store the indices array:
//     GLuint eab;
//     glGenBuffers(1, &eab);

//     // Transfer the data from indices to eab
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // Create a texture
//     GLuint texture;
//     glGenTextures(1, &texture);

//     // Specify that we work with a 2D texture
//     glBindTexture(GL_TEXTURE_2D, texture);

//     load_image("test.png");

//     //GLuint shaderProgram = create_program("shaders/vert.shader", "shaders/frag.shader");
//     GLuint shaderProgram = create_program(vert_shader, frag_shader);

//     // Get the location of the attributes that enters in the vertex shader
//     GLint position_attribute = glGetAttribLocation(shaderProgram, "position");

//     // Specify how the data for position can be accessed
//     glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

//     // Enable the attribute
//     glEnableVertexAttribArray(position_attribute);

//     // Texture coord attribute
//     GLint texture_coord_attribute = glGetAttribLocation(shaderProgram, "texture_coord");
//     glVertexAttribPointer(texture_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vertices_position));
//     glEnableVertexAttribArray(texture_coord_attribute);
// }

// Shader sources
const GLchar *vertexSource = R"glsl(
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

const GLchar *fragmentSource = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texKitten;
    uniform sampler2D texPuppy;
    void main()
    {
        outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
    }
)glsl";

class gl_render
{
public:
    gl_render()
    {
        _vao = 0;
        _vbo = 0;
    }

    void initialize()
    {
        // Create Vertex Array Object
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        // Create a Vertex Buffer Object and copy the vertex data to it
        glGenBuffers(1, &_vbo);

        GLfloat vertices[] = {
            //  Position      Color             Texcoords
            -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
            0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
            -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
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
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        // Link the vertex and fragment shader into a shader program
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);
        glUseProgram(shaderProgram);

        // Specify the layout of the vertex data
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glEnableVertexAttribArray(colAttrib);
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

        GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

        // Load textures
        GLuint textures[2];
        glGenTextures(2, textures);

        image img_from_file("test.png");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_from_file.get_width(), img_from_file.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_from_file.data());
        glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_from_file.get_width(), img_from_file.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_from_file.data());
        glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void render()
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    ~gl_render()
    {
        // glDeleteTextures(2, textures);

        // glDeleteProgram(shaderProgram);
        // glDeleteShader(fragmentShader);
        // glDeleteShader(vertexShader);

        glDeleteBuffers(1, &_ebo);
        glDeleteBuffers(1, &_vbo);

        glDeleteVertexArrays(1, &_vao);
    }

private:
    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class engine
{
public:
    engine()
    {
        _init_sdl();
        _set_glsl_version();
        _init_glew();
        _init_window();

        // Set some OpenGL settings
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
    }

    ~engine()
    {
        SDL_GL_DeleteContext(_context);
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    class render
    {
    public:
        render(engine &e) : _e(e)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        ~render()
        {
            SDL_GL_SwapWindow(_e._window);
        }

    private:
        engine &_e;
    };

    bool const &get_exit() const
    {
        return _exit;
    }

private:
    SDL_Window *_window;
    SDL_GLContext _context;
    bool _exit               = false;
    std::size_t _width       = 1024;
    std::size_t _height      = 1024;
    std::string _window_name = "Laboratory work 2";

    void _init_sdl()
    {
        /* SDL 2.0*/
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            std::string _error_message = "SDL2 initialize error: ";
            _error_message += SDL_GetError();
            throw std::runtime_error(_error_message);
        }
    }

    void _set_glsl_version()
    {
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }

    void _init_window()
    {
        _window = SDL_CreateWindow(_window_name.c_str(), 0, 0, _width, _height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if(!_window)
        {
            std::string _error_message = "SDL2 initialize window error: ";
            _error_message += SDL_GetError();
            throw std::runtime_error(_error_message);
        }
    }

    void _init_context()
    {
        _context = SDL_GL_CreateContext(_window);
        if(!_context)
        {
            std::string _error_message = "SDL2 initialize context error: ";
            _error_message += SDL_GetError();
            throw std::runtime_error(_error_message);
        }
    }

    void _init_glew()
    {
        // Enable glew experimental, this enables some more OpenGL extensions.
        glewExperimental   = GL_TRUE;
        GLenum _glew_error = glewInit();
        if(_glew_error != GLEW_OK)
        {
            std::string _error_message = "Failed to initialize GLEW: ";
            _error_message += (char *)glewGetErrorString(_glew_error);
            throw std::runtime_error(_error_message);
        }
    }

    void _event_handling()
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            _exit = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
                    break;
                case SDL_QUIT:
                    _exit = true;
                    break;
                default:
                    break;
            }
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    /* Signal handler */
    signal(SIGINT, signal_callback);

    try
    {
        engine e;
        while(!e.get_exit())
        {
            engine::render(*e);
        }
    }
    catch(std::exception const &e)
    {
        spdlog::error(e.what());
    }
    catch(...)
    {
        spdlog::error("Unexpected error");
    }

    return EXIT_SUCCESS;
}

// int main(int argc, char *argv[])
// {
//     /* Signal handler */
//     signal(SIGINT, signal_callback);

//     /* SDL 2.0*/
//     if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
//     {
//         spdlog::error("Error: {}", SDL_GetError());
//         return EXIT_FAILURE;
//     }

//     // GL 3.0 + GLSL 130
//     const char *glsl_version = "#version 130";
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

//     SDL_Window *window = SDL_CreateWindow("Laboratory work 2", 0, 0, 1024, 1024, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
//     if(!window)
//     {
//         spdlog::error("Error: {}", SDL_GetError());
//         return EXIT_FAILURE;
//     }
//     SDL_GLContext context = SDL_GL_CreateContext(window);
//     if(!context)
//     {
//         spdlog::error("Error: {}", SDL_GetError());
//         return EXIT_FAILURE;
//     }

//     // Enable glew experimental, this enables some more OpenGL extensions.
//     glewExperimental = GL_TRUE;
//     if(glewInit() != GLEW_OK)
//     {
//         spdlog::error("Failed to initialize GLEW");
//         return EXIT_FAILURE;
//     }

//     // Set some OpenGL settings
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glClearDepth(1.0f);

//     /* This */
//     glEnable(GL_DEPTH_TEST);

//     bool exit = false;

//     /* Image */
//     // image image_from_file("test.png");
//     gl_render glr;
//     glr.initialize();

//     SDL_Event event;
//     while(!exit)
//     {
//         while(SDL_PollEvent(&event))
//         {
//             switch(event.type)
//             {
//                 case SDL_KEYDOWN:
//                     switch(event.key.keysym.sym)
//                     {
//                         case SDLK_ESCAPE:
//                             exit = true;
//                             break;
//                         default:
//                             break;
//                     }
//                     break;
//                 case SDL_MOUSEBUTTONDOWN:
//                     spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
//                     break;
//                 case SDL_QUIT:
//                     exit = true;
//                     break;
//                 default:
//                     break;
//             }
//         }
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         glr.render();

//         SDL_GL_SwapWindow(window);
//     }

//     SDL_GL_DeleteContext(context);
//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return EXIT_SUCCESS;
// }

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