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

    /* This */
    glEnable(GL_DEPTH_TEST);

    bool exit = false;

    /* Image */
    image image_from_file("test.png");

    gpu g;
    g.run();
    spdlog::info("Exit");
    return EXIT_SUCCESS;

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