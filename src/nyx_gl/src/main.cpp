// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
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

        SDL_GL_SwapWindow(window);
    }

    /* SDL */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "main.h"

// #include <array>
// #include <cstddef>
// #include <cstdint>
// #include <cstdlib>

// // clang-format off
// #include <CL/cl_gl.h>
// #include <GL/glew.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>
// #include <GL/gl.h>
// // clang-format on

// // clang-format off
// #define CL_HPP_ENABLE_EXCEPTIONS
// #define CL_HPP_TARGET_OPENCL_VERSION  120
// #define CL_HPP_MINIMUM_OPENCL_VERSION 120

// #if defined(__APPLE__) || defined(__MACOSX)
// 	#include <OpenCL/cl.hpp>
// #else
// 	#include <CL/cl.h>
// #endif
// // clang-format on

// #include <CL/opencl.hpp>
// #include <SDL_events.h>
// #include <SDL_keycode.h>
// #include <SDL_touch.h>
// #include <cstdlib>
// #include <exception>
// #include <getopt.h>
// #include <glm/glm.hpp>
// #include <iostream>
// #include <iterator>
// #include <signal.h>
// #include <spdlog/spdlog.h>
// #include <stdexcept>
// #include <string>
// #include <unistd.h>
// #include <vector>

// #define BOOST_COMPUTE_CL_VERSION_1_2

// #include <boost/compute/algorithm/copy.hpp>
// #include <boost/compute/async/future.hpp>
// #include <boost/compute/command_queue.hpp>
// #include <boost/compute/container/vector.hpp>
// #include <boost/compute/event.hpp>
// #include <boost/compute/interop/opengl.hpp>
// #include <boost/compute/kernel.hpp>
// #include <boost/compute/program.hpp>
// #include <boost/compute/system.hpp>
// #include <boost/compute/utility/dim.hpp>
// #include <boost/compute/utility/source.hpp>

// class image_representation
// {
// public:
//     image_representation(std::size_t const &width, std::size_t const &height, std::size_t const &depth) : _width(width), _height(height), _depth(depth)
//     {
//         std::size_t size = width * height * depth;
//         _image.resize(size, 0);
//     }

//     std::size_t const &width() const
//     {
//         return _width;
//     }

//     std::size_t const &height() const
//     {
//         return _height;
//     }

//     std::size_t const &depth() const
//     {
//         return _depth;
//     }

//     std::size_t size()
//     {
//         return _image.size();
//     }

//     uint8_t *data()
//     {
//         return _image.data();
//     }

//     uint8_t const *const_data() const
//     {
//         return _image.data();
//     }

//     void fill_zeros()
//     {
//         for(std::size_t i = 0; i < _image.size(); i++)
//             _image[i] = 0;
//     }

//     void print()
//     {
//         spdlog::info("_width:        {}", _width);
//         spdlog::info("_height:       {}", _height);
//         spdlog::info("_depth:        {}", _depth);
//         spdlog::info("_image.size(): {}", _image.size());
//         spdlog::info("width:         {}", width());
//         spdlog::info("height:        {}", height());
//         spdlog::info("depth:         {}", depth());
//         spdlog::info("size:          {}", size());

//         std::string elements = "Elements: ";
//         for(std::size_t i = 0; i < 8; i++)
//             elements += std::to_string(_image[i]) + " ";
//         spdlog::info(elements);
//     }

// private:
//     std::size_t _width;
//     std::size_t _height;
//     std::size_t _depth;
//     std::vector<uint8_t> _image;
// };

// namespace compute = boost::compute;

// GLuint compile_shader(unsigned int type, const std::string &source)
// {
//     unsigned int id = glCreateShader(type);
//     const char *src = source.c_str();
//     glShaderSource(id, 1, &src, nullptr);
//     glCompileShader(id);

//     int result;
//     glGetShaderiv(id, GL_COMPILE_STATUS, &result);
//     if(result == GL_FALSE)
//     {
//         int length;
//         glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//         char *message = (char *)malloc(length);
//         glGetShaderInfoLog(id, length, &length, message);
//         std::string _error_message = "Compile shader error: ";
//         _error_message += message;
//         throw std::runtime_error(_error_message);
//         glDeleteShader(id);
//         return 0;
//     }
//     return id;
// }

// GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader)
// {
//     unsigned int program = glCreateProgram();
//     unsigned int vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
//     unsigned int fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

//     glAttachShader(program, vs);
//     glAttachShader(program, fs);
//     glLinkProgram(program);
//     glValidateProgram(program);

//     glDeleteShader(vs);
//     glDeleteShader(fs);

//     return program;
// }

// const GLchar *vertex_shader = R"glsl(
//     #version 150 core
//     in vec2 position;
//     in vec3 color;
//     in vec2 texcoord;
//     out vec3 Color;
//     out vec2 Texcoord;
//     void main()
//     {
//         Color = color;
//         Texcoord = texcoord;
//         gl_Position = vec4(position, 0.0, 1.0);
//     }
// )glsl";

// const GLchar *fragment_shader = R"glsl(
//     #version 150 core
//     in vec3 Color;
//     in vec2 Texcoord;
//     out vec4 outColor;
//     uniform sampler2D texKitten;
//     void main()
//     {
//         outColor = texture(texKitten, Texcoord);
//     }
// )glsl";

// std::string opencl_kernel = R"opencl_kernel(
// __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

// __kernel void example(
//     __read_only image2d_t img_in,
//     __write_only image2d_t img_out)
// {
//     const int x = get_global_id(0);
//     const int y = get_global_id(1);
//     int2 pos = (int2)(x, y);
//     uint4 pixel = read_imageui(img_in, sampler, pos);

//     if(pixel.y >= 255)
//         pixel.y = 0;

//     if(pixel.z >= 255)
//         pixel.z = 0;

//     // pixel.x += 10;
//     pixel.y += 10;
//     pixel.z += 10;
//     write_imageui(img_out, pos, pixel);
// }
// )opencl_kernel";

// std::string opencl_kernel_click_map = R"opencl_kernel(
// __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

// __kernel void click_map(
//     __read_only image2d_t img_in,
//     __write_only image2d_t img_out,
//     __global const int2 *clicks,
//     int clicks_size)
// {
//     const int x = get_global_id(0);
//     const int y = get_global_id(1);
//     int2 pos = (int2)(x, y);

//     for(int i = 0; i < clicks_size; i++)
//     {
//         if(all(clicks[i] == pos))
//         {
//             uint4 pixel = {255, 0, 255, 255};
//             write_imageui(img_out, pos, pixel);

//             // Area around the pixel
//             write_imageui(img_out, (int2)(pos.x + 1, pos.y + 1), pixel);
//             write_imageui(img_out, (int2)(pos.x + 1, pos.y + 0), pixel);
//             write_imageui(img_out, (int2)(pos.x + 0, pos.y + 1), pixel);
//             write_imageui(img_out, (int2)(pos.x - 1, pos.y - 1), pixel);
//             write_imageui(img_out, (int2)(pos.x - 1, pos.y + 0), pixel);
//             write_imageui(img_out, (int2)(pos.x + 0, pos.y - 1), pixel);
//             write_imageui(img_out, (int2)(pos.x + 1, pos.y - 1), pixel);
//             write_imageui(img_out, (int2)(pos.x - 1, pos.y + 1), pixel);
//         }
//     }
// }
// )opencl_kernel";

// void simple_kern()
// {
//     // get the default device
//     compute::device device = compute::system::default_device();

//     // create a context for the device
//     compute::context context(device);

//     // setup input arrays
//     float a[] = {1, 2, 3, 4};
//     float b[] = {5, 6, 7, 8};

//     // make space for the output
//     float c[] = {0, 0, 0, 0};

//     // create memory buffers for the input and output
//     compute::buffer buffer_a(context, 4 * sizeof(float));
//     compute::buffer buffer_b(context, 4 * sizeof(float));
//     compute::buffer buffer_c(context, 4 * sizeof(float));

//     // source code for the add kernel
//     const char source[] = "__kernel void add(__global const float *a,"
//                           "                  __global const float *b,"
//                           "                  __global float *c)"
//                           "{"
//                           "    const uint i = get_global_id(0);"
//                           "    c[i] = a[i] + b[i];"
//                           "}";

//     // create the program with the source
//     compute::program program = compute::program::create_with_source(source, context);

//     // compile the program
//     program.build();

//     // create the kernel
//     compute::kernel kernel(program, "add");

//     // set the kernel arguments
//     kernel.set_arg(0, buffer_a);
//     kernel.set_arg(1, buffer_b);
//     kernel.set_arg(2, buffer_c);

//     // create a command queue
//     compute::command_queue queue(context, device);

//     // write the data from 'a' and 'b' to the device
//     queue.enqueue_write_buffer(buffer_a, 0, 4 * sizeof(float), a);
//     queue.enqueue_write_buffer(buffer_b, 0, 4 * sizeof(float), b);

//     // run the add kernel
//     queue.enqueue_1d_range_kernel(kernel, 0, 4, 0);

//     // transfer results back to the host array 'c'
//     queue.enqueue_read_buffer(buffer_c, 0, 4 * sizeof(float), c);

//     // print out results in 'c'
//     spdlog::info("c: [{}, {}, {}, {}]", c[0], c[1], c[2], c[3]);
// }

// void test_kern()
// {
//     // source code for the add kernel
//     std::string source = R"cl(
// __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

// __kernel void click_map(
//     __read_only image2d_t img_in,
//     __write_only image2d_t img_out,
//     __global const int2 *clicks,
//     int clicks_size)
// {
//     const int x = get_global_id(0);
//     const int y = get_global_id(1);
//     int2 pos = (int2)(x, y);
// }
// )cl";

//     int window_width  = 1024;
//     int window_height = 1024;

//     // get the default device
//     compute::device device = compute::system::default_device();

//     // create a context for the device
//     compute::context context(device);

//     // image2d format
//     compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

//     /* Variables */
//     image_representation ir_click_map(window_width, window_height, 4);
//     ir_click_map.fill_zeros();
//     ir_click_map.print();

//     std::vector<compute::int2_> vector_click_map;
//     vector_click_map.push_back({20, 20});
//     vector_click_map.push_back({40, 40});
//     vector_click_map.push_back({60, 60});
//     vector_click_map.push_back({80, 80});

//     // build program
//     compute::program program = compute::program::create_with_source(opencl_kernel_click_map, context);

//     try
//     {
//         program.build();
//     }
//     catch(std::exception const &e)
//     {
//         spdlog::error("OpenCL build error: {}", e.what());
//         spdlog::error("OpenCL build log: \n{}", program.build_log());
//     }
//     catch(...)
//     {
//         spdlog::error("OpenCL unexpected build error");
//     }

//     // setup kernel
//     compute::kernel kernel_cl_click_map(program, "click_map");

//     // create input and output images on the gpu
//     compute::image2d img_2d_in_click_map(context, ir_click_map.width(), ir_click_map.height(), format, compute::image2d::read_only);
//     compute::image2d img_2d_out_click_map(context, ir_click_map.width(), ir_click_map.height(), format, compute::image2d::write_only);
//     compute::buffer buffer_vector_click_map(context, vector_click_map.size() * sizeof(compute::int2_), compute::buffer::read_only);

//     // set args
//     try
//     {
//         kernel_cl_click_map.set_arg(0, img_2d_in_click_map);
//         kernel_cl_click_map.set_arg(1, img_2d_out_click_map);
//         kernel_cl_click_map.set_arg(2, buffer_vector_click_map);
//         kernel_cl_click_map.set_arg(3, (int)vector_click_map.size());
//     }
//     catch(std::exception const &e)
//     {
//         spdlog::error("OpenCL set_arg error: {}", e.what());
//     }
//     catch(...)
//     {
//         spdlog::error("OpenCL unexpected set_arg error");
//     }

//     // regions
//     std::size_t origin[3] = {0, 0, 0};
//     std::size_t region[3] = {window_width, window_height, 1};

//     // create a command queue
//     compute::command_queue queue(context, device);

//     // write buffers
//     queue.enqueue_write_image(img_2d_in_click_map, img_2d_in_click_map.origin(), img_2d_in_click_map.size(), ir_click_map.const_data());
//     queue.enqueue_write_buffer(buffer_vector_click_map, 0, vector_click_map.size() * sizeof(compute::int2_), vector_click_map.data());
//     queue.finish();

//     // compute
//     queue.enqueue_nd_range_kernel(kernel_cl_click_map, 2, origin, region, 0);
//     queue.finish();

//     // read data from device
//     queue.enqueue_read_image(img_2d_out_click_map, origin, region, 0, 0, ir_click_map.data());
//     queue.finish();

//     for(std::size_t i = 0; i < ir_click_map.size(); i++)
//     {
//         if(ir_click_map.data()[i] != 0)
//             spdlog::info("ir_click_map[{}]: {}", i, ir_click_map.data()[i]);
//     }
// }

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

//     /* SDL variables */
//     int window_width  = 0;
//     int window_height = 0;
//     SDL_GetWindowSize(window, &window_width, &window_height);
//     spdlog::info("Window width: {} height: {}", window_width, window_height);

//     /* Some OpenGL settings */
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glClearDepth(1.0f);
//     glEnable(GL_DEPTH_TEST);

//     /* OpenGL */
//     GLuint _vao = 0;
//     GLuint _vbo = 0;
//     GLuint _ebo = 0;

//     // Create Vertex Array Object
//     glGenVertexArrays(1, &_vao);
//     glBindVertexArray(_vao);

//     // Create a Vertex Buffer Object and copy the vertex data to it
//     glGenBuffers(1, &_vbo);

//     GLfloat vertices[] = {
//         //  Position      Color             Texcoords
//         -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
//         1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
//         1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
//         -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
//     };

//     glBindBuffer(GL_ARRAY_BUFFER, _vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // Create an element array
//     glGenBuffers(1, &_ebo);

//     GLuint elements[] = {0, 1, 2, 2, 3, 0};

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

//     // Create and compile the vertex shader
//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertex_shader, NULL);
//     glCompileShader(vertexShader);

//     // Create and compile the fragment shader
//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
//     glCompileShader(fragmentShader);

//     // Link the vertex and fragment shader into a shader program
//     GLuint shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glBindFragDataLocation(shaderProgram, 0, "outColor");
//     glLinkProgram(shaderProgram);
//     glUseProgram(shaderProgram);

//     // Specify the layout of the vertex data
//     GLint pos_attribute = glGetAttribLocation(shaderProgram, "position");
//     glEnableVertexAttribArray(pos_attribute);
//     glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

//     GLint col_attribute = glGetAttribLocation(shaderProgram, "color");
//     glEnableVertexAttribArray(col_attribute);
//     glVertexAttribPointer(col_attribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

//     GLint tex_attribute = glGetAttribLocation(shaderProgram, "texcoord");
//     glEnableVertexAttribArray(tex_attribute);
//     glVertexAttribPointer(tex_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

//     // Load textures
//     std::array<GLuint, 1> textures;
//     glGenTextures(1, textures.data());

//     image_representation ir(window_width, window_height, 4);
//     for(std::size_t i = 0, rgb = 0; i < ir.size(); i++, rgb++)
//     {
//         switch(rgb)
//         {
//             case 0:
//                 ir.data()[i] = 0;
//                 break;
//             case 1:
//                 ir.data()[i] = 255;
//                 break;
//             case 2:
//                 ir.data()[i] = 239;
//                 break;
//             case 3:
//                 ir.data()[i] = 255;
//                 break;
//             default:
//                 rgb = 0;
//                 break;
//         }
//     }

//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, textures[0]);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ir.width(), ir.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.data());
//     glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     /* OpenCL */

//     // get the default device
//     compute::device gpu_cl = compute::system::default_device();
//     spdlog::info("OpenCL device: {}", gpu_cl.name());

//     // create context for default device
//     compute::context context_cl(gpu_cl);

//     // create command queue
//     compute::command_queue queue_cl(context_cl, gpu_cl);

//     // build program
//     compute::program program_cl = compute::program::create_with_source(opencl_kernel, context_cl);
//     program_cl.build();

//     // image2d format
//     compute::image_format format_cl(CL_RGBA, CL_UNSIGNED_INT8);

//     // create input and output images on the gpu
//     compute::image2d img_2d_in(context_cl, ir.width(), ir.height(), format_cl, compute::image2d::read_only);
//     compute::image2d img_2d_out(context_cl, ir.width(), ir.height(), format_cl, compute::image2d::write_only);

//     // fill buffer
//     queue_cl.enqueue_write_image(img_2d_in, img_2d_in.origin(), img_2d_in.size(), ir.const_data());

//     // setup tesselate_sphere kernel
//     compute::kernel kernel_cl(program_cl, "example");

//     // set args
//     kernel_cl.set_arg<compute::image2d>(0, img_2d_in);
//     kernel_cl.set_arg<compute::image2d>(1, img_2d_out);

//     // regions
//     std::size_t origin[3] = {0, 0, 0};
//     std::size_t region[3] = {ir.width(), ir.height(), 1};

//     try
//     {
//         simple_kern();
//     }
//     catch(std::exception const &e)
//     {
//         spdlog::error("simple_kern() error: {}", e.what());
//     }
//     catch(...)
//     {
//         spdlog::error("Unexpected simple_kern() error");
//     }

//     try
//     {
//         test_kern();
//     }
//     catch(std::exception const &e)
//     {
//         spdlog::error("test_kern() error: {}", e.what());
//     }
//     catch(...)
//     {
//         spdlog::error("Unexpected test_kern() error");
//     }

//     /* Main loop */
//     bool exit = false;
//     SDL_Event event;
//     while(!exit)
//     {
//         // fill
//         queue_cl.enqueue_write_image(img_2d_in, img_2d_in.origin(), img_2d_in.size(), ir.const_data());
//         queue_cl.finish();

//         // compute
//         queue_cl.enqueue_nd_range_kernel(kernel_cl, 2, origin, region, 0);
//         queue_cl.finish();

//         // read data from device
//         queue_cl.enqueue_read_image(img_2d_out, origin, region, 0, 0, ir.data());
//         queue_cl.finish();

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

//         /* Update texture */
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ir.width(), ir.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.data());

//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         /* Draw */
//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         SDL_GL_SwapWindow(window);
//     }
//     /* OpenGL */
//     glDeleteBuffers(1, &_ebo);
//     glDeleteBuffers(1, &_vbo);
//     glDeleteVertexArrays(1, &_vao);
//     glDeleteTextures(1, textures.data());

//     /* SDL */
//     SDL_GL_DeleteContext(context);
//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return EXIT_SUCCESS;
// }

// void signal_callback(int signum)
// {
//     spdlog::info("Signal: {}", signum);

//     switch(signum)
//     {
//         case 2:
//             spdlog::info("Signal SIGINT. Exiting from the application");
//             break;
//         case 3:
//             spdlog::info("Signal SIGQUIT. Exiting from the application");
//             break;
//         case 10:
//             spdlog::error("Signal SIGBUS. Bus error");
//             break;
//         case 13:
//             spdlog::error("Signal SIGPIPE. Write on a pipe with no one to read it");
//             break;
//         default:
//             spdlog::warn("Unspecified signal: {}", signum);
//             break;
//     }
//     exit(signum);
// }