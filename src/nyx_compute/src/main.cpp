#include "main.h"

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

    image(
        std::size_t const &width,
        std::size_t const &height,
        std::size_t const &channels,
        IMAGE_TYPE const &image_type,
        std::vector<uint8_t> const &image)
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

    std::size_t const get_width() const
    {
        return _width;
    }

    std::size_t const get_height() const
    {
        return _height;
    }

    std::size_t const get_channels() const
    {
        return _channels;
    }

    std::size_t const get_pixels() const
    {
        return _image.size();
    }

    std::size_t const size() const
    {
        return _image.size();
    }

    std::vector<uint8_t> const get_rgb() const
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

    std::vector<uint8_t> const get_rgba() const
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

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _channels;
    std::size_t _pixels;
    IMAGE_TYPE _image_type;
    std::vector<pixel> _image;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BOOST_COMPUTE_CL_VERSION_1_2

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/async/future.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/event.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/dim.hpp>

namespace compute = boost::compute;

void copy_custom_image_to_image(image const &img, compute::image2d &img_ret, compute::command_queue &queue = compute::system::default_queue())
{
    std::vector<uint8_t> image_vec = img.get_rgba();

    queue.enqueue_write_image(img_ret, img_ret.origin(), img_ret.size(), image_vec.data());
}

/* Create image */
compute::image2d custom_image_create_image2d(image const &img, cl_mem_flags flags, compute::command_queue &queue = compute::system::default_queue())
{
    compute::context const &context = queue.get_context();
    compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

    compute::image2d image_ret(context, img.get_width(), img.get_height(), format, flags);

    copy_custom_image_to_image(img, image_ret, queue);

    return image_ret;
}

/* Copy image from device to image */
void copy_image_from_device_to_image(compute::image2d const &img, image &image_ret, compute::command_queue &queue)
{
    // queue.enqueue_read_image(img,)
}

class gpu
{
public:
    void run()
    {
        /* Image */
        image image_from_file("test.png");

        /* Buffers */
        std::vector<uint8_t> data_image_in(image_from_file.size() * 4);
        std::vector<uint8_t> data_image_out(image_from_file.size() * 4);

        /* Fill buffer data_image_in with image */
        for(std::size_t i = 0, fl = 0; i < image_from_file.size(); i++, fl += 4)
        {
            data_image_in[fl + 0] = image_from_file.get()[i].r;
            data_image_in[fl + 1] = image_from_file.get()[i].g;
            data_image_in[fl + 2] = image_from_file.get()[i].b;
            data_image_in[fl + 3] = image_from_file.get()[i].a;
        }

        // std::string kernel_source =
        //     BOOST_COMPUTE_STRINGIZE_SOURCE(__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

        //                                    __kernel void example(__read_only image2d_t img_in, __write_only image2d_t img_out) {
        //                                        const int x = get_global_id(0);
        //                                        const int y = get_global_id(1);
        //                                        int2 pos    = (int2)(x, y);
        //                                        uint4 pixel = read_imageui(img_in, sampler, pos);
        //                                        pixel.x += 50;
        //                                        pixel.y += 50;
        //                                        pixel.z += 50;
        //                                        write_imageui(img_out, pos, pixel);
        //                                    });
        std::string kernel_source = {"__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;"
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

        // get the default device
        compute::device gpu = compute::system::default_device();

        // create context for default device
        compute::context context(gpu);

        // create command queue
        compute::command_queue queue(context, gpu);

        // build program
        compute::program program = compute::program::create_with_source(kernel_source, context);
        program.build();

        /* Create image types */
        // cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);
        // cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, image_from_file.get_width(), image_from_file.get_height(), 0, &data_image_in[0]);
        // cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, image_from_file.get_width(), image_from_file.get_height(), 0, NULL);
        compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

        // compute::image2d img_2d_in(context, );
        compute::image2d img_2d_in = custom_image_create_image2d(image_from_file, compute::image2d::read_only, queue);
        compute::image2d img_2d_out(context, image_from_file.get_width(), image_from_file.get_height(), format, compute::image2d::write_only);

        // setup tesselate_sphere kernel
        compute::kernel kernel(program, "example");

        kernel.set_arg<compute::image2d>(0, img_2d_in);
        kernel.set_arg<compute::image2d>(1, img_2d_out);

        // std::array<cl::size_type, 3> origin {0, 0, 0};
        // std::array<cl::size_type, 3> region {image_from_file.get_width(), image_from_file.get_height(), 1};
        std::size_t origin[3] = {0, 0, 0};
        std::size_t region[3] = {image_from_file.get_width(), image_from_file.get_height(), 1};

        // queue.enqueue_nd_range_kernel(kernel, 2, origin, region, 0);
        queue.enqueue_nd_range_kernel(kernel, 2, origin, region, 0);

        /* Is it necessary? */
        queue.finish();

        /* Enqueue? */

        /* Get image from buffer */
        // queue.enqueue_read_image(img_2d_out, origin, region, data_image_out.size(), data_image_out.data());
        // queue.enqueue_read_image(img_2d_out, origin, region, image_from_file.get_width(), image_from_file.get_height(), data_image_out.data());
        queue.enqueue_read_image(img_2d_out, origin, region, 0, 0, data_image_out.data());

        /* Create output image */
        image image_out(
            image_from_file.get_width(), image_from_file.get_height(), image_from_file.get_channels(), image::IMAGE_TYPE::RGBA, &data_image_out[0]);
        stbi_write_png("out.png", image_out.get_width(), image_out.get_height(), 3, image_out.get_rgb().data(), image_out.get_width() * 3);
    }

private:
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/compute/command_queue.hpp>
#include <boost/compute/interop/opengl.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/program.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>

// opencl source code
const char source[] = BOOST_COMPUTE_STRINGIZE_SOURCE(
    // map value to color
    float4 color(uint i) {
        uchar c   = i;
        uchar x   = 35;
        uchar y   = 25;
        uchar z   = 15;
        uchar max = 255;

        if(i == 256)
            return (float4)(0, 0, 0, 255);
        else
            return (float4)(max - x * i, max - y * i, max - z * i, max) / 255.f;
    }

    __kernel void mandelbrot(__write_only image2d_t image) {
        const uint x_coord = get_global_id(0);
        const uint y_coord = get_global_id(1);
        const uint width   = get_global_size(0);
        const uint height  = get_global_size(1);

        float x_origin = ((float)x_coord / width) * 3.25f - 2.0f;
        float y_origin = ((float)y_coord / height) * 2.5f - 1.25f;

        float x = 0.0f;
        float y = 0.0f;

        uint i = 0;
        while(x * x + y * y <= 4.f && i < 256)
        {
            float tmp = x * x - y * y + x_origin;
            y         = 2 * x * y + y_origin;
            x         = tmp;
            i++;
        }

        int2 coord = {x_coord, y_coord};
        write_imagef(image, coord, color(i));
    };);

float width()
{
    return 200;
}

float height()
{
    return 200;
}

class mandelbrot
{
public:
    mandelbrot()
    {
        _gl_texture = 0;
    }

    void initialize_gl()
    {
        // setup opengl
        glDisable(GL_LIGHTING);

        // create the OpenGL/OpenCL shared context
        _context = compute::opengl_create_shared_context();

        // get gpu device
        compute::device gpu = _context.get_device();
        spdlog::info("device: {}", gpu.name());

        // setup command queue
        _queue = compute::command_queue(_context, gpu);

        // build mandelbrot program
        _program = compute::program::create_with_source(source, _context);
        _program.build();
    }

    void resize_gl(std::size_t width, std::size_t height)
    {
        // resize viewport
        glViewport(0, 0, width, height);

        // delete old texture
        if(_gl_texture)
        {
            glDeleteTextures(1, &_gl_texture);
            _gl_texture = 0;
        }

        // generate new texture
        glGenTextures(1, &_gl_texture);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        // create opencl object for the texture
        _cl_texture = compute::opengl_texture(_context, GL_TEXTURE_2D, 0, _gl_texture, CL_MEM_WRITE_ONLY);
    }

    void paint_gl()
    {
        using compute::dim;

        float w = width();
        float h = height();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // setup the mandelbrot kernel
        compute::kernel kernel(_program, "mandelbrot");
        kernel.set_arg(0, _cl_texture);

        // acquire the opengl texture so it can be used in opencl
        compute::opengl_enqueue_acquire_gl_objects(1, &_cl_texture.get(), _queue);

        // execute the mandelbrot kernel
        _queue.enqueue_nd_range_kernel(kernel, dim(0, 0), dim(width(), height()), dim(1, 1));

        // release the opengl texture so it can be used by opengl
        compute::opengl_enqueue_release_gl_objects(1, &_cl_texture.get(), _queue);

        // ensure opencl is finished before rendering in opengl
        _queue.finish();

        // draw a single quad with the mandelbrot image texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);
        glTexCoord2f(0, 1);
        glVertex2f(0, h);
        glTexCoord2f(1, 1);
        glVertex2f(w, h);
        glTexCoord2f(1, 0);
        glVertex2f(w, 0);
        glEnd();
    }

    GLuint const &get_texture() const
    {
        return _gl_texture;
    }

    // Side : res[0] = width; res[1] = height;
    float *get_scale_normalized()
    {
        float *res = (float *)malloc(2 * sizeof(float));
        if(width() > height())
        {
            res[0] = 1;
            res[1] = (float)height() / (float)width();
        }
        else
        {
            res[1] = 1;
            res[0] = (float)width() / (float)height();
        }
        return res;
    }

    void nothing() {}

private:
    compute::context _context;
    compute::command_queue _queue;
    compute::program _program;
    GLuint _gl_texture;
    compute::opengl_texture _cl_texture;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint compile_shader(unsigned int type, const std::string &source);
GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader);

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
        throw std::runtime_error("Compile shader error");
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

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        spdlog::error("{}", &log[0]);
        return false;
    }
    return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        spdlog::error("{}", &log[0]);
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    mandelbrot m;
    m.initialize_gl();

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

    /* This */
    glEnable(GL_DEPTH_TEST);

    bool exit = false;

    /* Image */
    // image image_from_file("test.png");

    // gpu g;
    // g.run();

    /* OpenGL */
    // Shaders
    std::string vert_shader = "#version 330 core\n"
                              "\n"
                              "layout(location = 0) in vec4 position;\n"
                              "layout(location = 1) in vec2 texCoord;\n"
                              "\n"
                              "out vec2 v_TexCoord;\n"
                              "uniform mat4 u_MVP;\n"
                              "\n"
                              "void main() {\n"
                              "	v_TexCoord = texCoord;\n"
                              "	gl_Position = position;\n"
                              "}\n";

    std::string frag_shader = "#version 330 core\n"
                              "\n"
                              "layout(location = 0) out vec4 color;\n"
                              "\n"
                              "in vec2 v_TexCoord;\n"
                              "\n"
                              "uniform sampler2D u_Texture;\n"
                              "\n"
                              "void main()\n"
                              "{\n"
                              "	color = texture(u_Texture, v_TexCoord);\n"
                              "}\n";

    GLuint texture_shader;
    try
    {
        texture_shader = create_program(vert_shader, frag_shader);
    }
    catch(std::exception &e)
    {
        spdlog::error("Error with creating program: {}", e.what());
    }
    glUseProgram(texture_shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // texture texture("test.png");
    // texture.bind();
    // bind
    //glActiveTexture(GL_TEXTURE0 + slot);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m.get_texture());
    // unbind
    // glBindTexture(GL_TEXTURE_2D, 0);

    glUniform1i(glGetUniformLocation(texture_shader, "u_Texture"), 0); // glUseProgram(redP_shader);

    //Scaling the Verts:
    float *scale = m.get_scale_normalized();

    // Verts
    float h = scale[1], w = scale[0];
    float positions[] = {
        -w,
        -h,
        0.0,
        0.0,
        -w,
        h,
        0.0,
        1.0,
        w,
        -h,
        1.0,
        0.0,
        //
        w,
        h,
        1.0,
        1.0};

    GLuint indices[] {0, 1, 2, 1, 2, 3};

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (char *)0 + 0 * sizeof(GLfloat));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (char *)0 + 2 * sizeof(GLfloat));

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

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

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    spdlog::info("Exit");
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