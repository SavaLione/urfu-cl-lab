#include "main.h"

#include "SDL2/SDL_video.h"
#include "glm/fwd.hpp"

#include <array>
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

namespace compute = boost::compute;

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

// Shader sources
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

// std::string kernel_source = {"__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;"
//                              ""
//                              "__kernel void example("
//                              "    __read_only image2d_t img_in,"
//                              "    __write_only image2d_t img_out)"
//                              "{"
//                              "    const int x = get_global_id(0);"
//                              "    const int y = get_global_id(1);"
//                              "    int2 pos = (int2)(x, y);"
//                              "    uint4 pixel = read_imageui(img_in, sampler, pos);"
//                              "    pixel.x += 50;"
//                              "    pixel.y += 50;"
//                              "    pixel.z += 50;"
//                              "    write_imageui(img_out, pos, pixel);"
//                              "}"
//                              ""
//                              ""};

// std::string const opencl_kernel = R"opencl_kernel(
// __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;"

// __kernel void example(
//     __read_only image2d_t img_in,
//     __write_only image2d_t img_out)
// {
//     const int x = get_global_id(0);
//     const int y = get_global_id(1);
//     int2 pos = (int2)(x, y);
//     uint4 pixel = read_imageui(img_in, sampler, pos);
//     pixel.x += 50;
//     pixel.y += 50;
//     pixel.z += 50;
//     write_imageui(img_out, pos, pixel);
// }
// )opencl_kernel";

/* Original kernel */
// std::string const opencl_kernel = R"opencl_kernel(
//     // map value to color
//     float4 color(uint i)
//     {
//         uchar c = i;
//         uchar x = 35;
//         uchar y = 25;
//         uchar z = 15;
//         uchar max = 255;

//         if(i == 256)
//             return (float4)(0, 0, 0, 255);
//         else
//             return (float4)(max-x*i, max-y*i, max-z*i, max) / 255.f;
//     }

//     __kernel void mandelbrot(__write_only image2d_t image)
//     {
//         const uint x_coord = get_global_id(0);
//         const uint y_coord = get_global_id(1);
//         const uint width = get_global_size(0);
//         const uint height = get_global_size(1);

//         float x_origin = ((float) x_coord / width) * 3.25f - 2.0f;
//         float y_origin = ((float) y_coord / height) * 2.5f - 1.25f;

//         float x = 0.0f;
//         float y = 0.0f;

//         uint i = 0;
//         while(x*x + y*y <= 4.f && i < 256){
//             float tmp = x*x - y*y + x_origin;
//             y = 2*x*y + y_origin;
//             x = tmp;
//             i++;
//         }

//         int2 coord = { x_coord, y_coord };
//         write_imagef(image, coord, color(i));
//     };
// )opencl_kernel";

std::string const opencl_kernel = R"opencl_kernel(
    // map value to color
    uint4 color(uint i)
    {
        uint c = i;
        uint x = 35;
        uint y = 25;
        uint z = 15;
        uint max = 255;

        if(i == 256)
            return (uint4)(0, 0, 0, 255);
        else
            return (uint4)(max-x*i, max-y*i, max-z*i, max) / 255;
    }

    __kernel void mandelbrot(__write_only image2d_t image)
    {
        const uint x_coord = get_global_id(0);
        const uint y_coord = get_global_id(1);
        const uint width = get_global_size(0);
        const uint height = get_global_size(1);

        uint x_origin = (x_coord / width) * 3 - 2;
        uint y_origin = (y_coord / height) * 2 - 1;

        uint x = 0;
        uint y = 0;

        uint i = 0;
        while(x*x + y*y <= 4 && i < 256){
            uint tmp = x*x - y*y + x_origin;
            y = 2*x*y + y_origin;
            x = tmp;
            i++;
        }

        int2 coord = { x_coord, y_coord };
        write_imageui(image, coord, color(i));
    };
)opencl_kernel";

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

    // Set some OpenGL settings
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    /* Setup OpenGL (for OpenCL) */
    glDisable(GL_LIGHTING);

    /* OpenCL */

    // default OpenCL device
    // compute::device gpu = compute::system::default_device();

    // create context for default device
    // compute::context context_cl(gpu);
    compute::context context_cl = compute::opengl_create_shared_context();

    /* default OpenCL device */
    compute::device gpu = context_cl.get_device();

    spdlog::info("OpenCL/OpenGL device: {}", gpu.name());

    // create command queue
    compute::command_queue queue(context_cl, gpu);

    // build program
    compute::program program_cl = compute::program::create_with_source(opencl_kernel, context_cl);
    program_cl.build();

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

    image img_from_file("test.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_from_file.get_width(), img_from_file.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_from_file.data());
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* OpenCL */
    compute::opengl_texture cl_texture_;
    GLuint gl_texture_;

    using compute::dim;

    int window_width  = 0;
    int window_height = 0;

    SDL_GetWindowSize(window, &window_width, &window_height);

    spdlog::info("SDL window size width: {} height: {}", window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, 0.0, window_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // setup the mandelbrot kernel
    compute::kernel kernel(program_cl, "mandelbrot");
    kernel.set_arg(0, cl_texture_);

    // acquire the opengl texture so it can be used in opencl
    compute::opengl_enqueue_acquire_gl_objects(1, &cl_texture_.get(), queue);

    // execute the mandelbrot kernel
    queue.enqueue_nd_range_kernel(kernel, dim(0, 0), dim(window_width, window_height), dim(1, 1));

    // release the opengl texture so it can be used by opengl
    compute::opengl_enqueue_release_gl_objects(1, &cl_texture_.get(), queue);

    // ensure opencl is finished before rendering in opengl
    queue.finish();

    // draw a single quad with the mandelbrot image texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gl_texture_);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(0, 1);
    glVertex2f(0, window_height);
    glTexCoord2f(1, 1);
    glVertex2f(window_width, window_height);
    glTexCoord2f(1, 0);
    glVertex2f(window_width, 0);
    glEnd();

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

    /* OpenGL */
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);

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