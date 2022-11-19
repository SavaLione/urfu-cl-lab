// // clang-format off
// #include <cstdint>
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"
// // clang-format on

// #include <CL/opencl.hpp>
// #include <cstdlib>
// #include <exception>
// #include <getopt.h>
// #include <iostream>
// #include <iterator>
// #include <signal.h>
// #include <spdlog/spdlog.h>
// #include <stdexcept>
// #include <string>
// #include <unistd.h>
// #include <vector>

// struct pixel
// {
//     uint8_t r;
//     uint8_t g;
//     uint8_t b;
//     uint8_t a;
// };

// class image
// {
// public:
//     image(std::string const &filename)
//     {
//         int image_width    = 0;
//         int image_height   = 0;
//         int image_channels = 0;

//         unsigned char *img = stbi_load(filename.c_str(), &image_width, &image_height, &image_channels, 0);

//         if(img == NULL)
//         {
//             std::string err = "Can't load " + filename + " image";
//             throw std::runtime_error(err);
//         }

//         _width    = image_width;
//         _height   = image_height;
//         _channels = image_channels;
//         _pixels   = image_width * image_height;

//         spdlog::info("channels: {}", _channels);

//         if(_channels == 3)
//             for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 3)
//                 _image.push_back({img[fl + 0], img[fl + 1], img[fl + 2], 255});
//         if(_channels == 4)
//             for(std::size_t i = 0, fl = 0; i < _pixels; i++, fl += 4)
//                 _image.push_back({img[fl + 0], img[fl + 1], img[fl + 2], img[fl + 3]});

//         stbi_image_free(img);
//     }

//     std::vector<uint8_t> get()
//     {
//         std::vector<uint8_t> ret;
//         for(std::size_t i = 0; i < _image.size(); i++)
//         {
//             ret.push_back(_image[i].r);
//             ret.push_back(_image[i].g);
//             ret.push_back(_image[i].b);
//         }
//         return ret;
//     }

// private:
//     std::vector<pixel> _image;
//     std::size_t _width;
//     std::size_t _height;
//     std::size_t _channels;
//     std::size_t _pixels;
// };

// int main()
// {
//     image img("test.png");

//     stbi_write_png("img.png", 1024, 1024, 3, img.get().data(), 1024 * 3);
//     return 0;
// }

#include <array>
#include <cstdint>
#include <cstdlib>
// clang-format off
#include <CL/cl_gl.h>
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

// typedef union
// {
//     uint8_t _ALIGNED(4) s[4];

//     __ANON_STRUCT__ struct
//     {
//         uint8_t r, g, b, a;
//     };
//     __ANON_STRUCT__ struct
//     {
//         uint8_t s0, s1, s2, s3;
//     };
//     __ANON_STRUCT__ struct
//     {
//         uint8_t x, y, z, w;
//     };
// } pixel;

struct pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

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
        return _height * _width;
    }

    std::size_t size()
    {
        return _image.size();
    }

    std::vector<uint8_t> get_rgb()
    {
        std::vector<uint8_t> ret;
        // for(std::size_t i = 0; i < _image.size(); i++)
        // {
        //     ret.push_back(_image[i].r);
        //     ret.push_back(_image[i].g);
        //     ret.push_back(_image[i].b);
        // }
        for(std::size_t i = 0; i < _height * _width; i++)
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
        for(std::size_t i = 0; i < _height * _width; i++)
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

    void info()
    {
        spdlog::info("_width:               {}", _width);
        spdlog::info("_height:              {}", _height);
        spdlog::info("_channels:            {}", _channels);
        spdlog::info("_pixels:              {}", _pixels);
        // spdlog::info("get_size_rgb():       {}", get_size_rgb());
        // spdlog::info("get_size_rgba():      {}", get_size_rgba());
        spdlog::info("rgb():                {}", size() * 3);
        spdlog::info("rgba():               {}", size() * 4);
        spdlog::info("_image.size():        {}", _image.size());
        spdlog::info("get_rgb().size():     {}", get_rgb().size());
        spdlog::info("get_rgba().size():    {}", get_rgba().size());
    }

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _channels;
    std::size_t _pixels;
    IMAGE_TYPE _image_type;
    std::vector<pixel> _image;
};

int main()
{
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
                          "    write_imageui(img_out, pos, pixel);"
                          "}"
                          ""
                          ""};

    kernels.push_back(kernel);

    try
    {
        image img("test.png");
        img.info();

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
            return EXIT_FAILURE;
        }

        cl::CommandQueue queue(context, default_device, 0, NULL);

        cl::Kernel kernel_simple_add(program, "example");
        cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);

        // uint8_t data_image_in[img.get_size_rgba()];
        // uint8_t data_image_out[img.get_size_rgba()];
        // std::vector<uint8_t> data_image_in = img.get_rgba();
        // std::vector<uint8_t> data_image_out(img.get_size_rgba());
        std::vector<uint8_t> data_image_in = img.get_rgba();
        std::vector<uint8_t> data_image_out;
        data_image_out.resize(img.size() * 4);

        //std::copy(img.get_rgba().begin(), img.get_rgba().end(), data_image_in);

        // cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, img.get_width(), img.get_height(), 0, &data_image_in[0]);
        cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, img.get_width(), img.get_height(), 0, data_image_in.data());
        cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, img.get_width(), img.get_height(), 0, NULL);

        cl::NDRange global(img.get_width(), img.get_height());
        // std::array<cl::size_type, 3> origin {0, 0, 0};
        // std::array<cl::size_type, 3> region {16, 16, 1};
        std::array<cl::size_type, 3> origin;
        origin[0] = 0;
        origin[1] = 0;
        origin[2] = 0;
        std::array<cl::size_type, 3> region;
        region[0] = 16;
        region[1] = 16;
        region[2] = 1;
        // cl::size_type origin[3] = {0, 0, 0};
        // cl::size_type region[3] = {16, 16, 1};

        kernel_simple_add.setArg(0, img_2d_in);
        kernel_simple_add.setArg(1, img_2d_out);

        queue.enqueueNDRangeKernel(kernel_simple_add, cl::NullRange, cl::NDRange(16, 16), cl::NullRange);
        queue.finish();

        queue.enqueueReadImage(img_2d_out, CL_TRUE, origin, region, 0, 0, data_image_out.data());

        //image image_out(img.get_width(), img.get_height(), img.get_channels(), image::IMAGE_TYPE::RGBA, data_image_out);
        // image image_out(img.get_width(), img.get_height(), img.get_channels(), image::IMAGE_TYPE::RGBA, data_image_out);
        image image_out(img.get_width(), img.get_height(), img.get_channels(), image::IMAGE_TYPE::RGBA, data_image_in);
        image_out.info();

        // stbi_write_png(
        //     "image_out.png",
        //     image_out.get_width(),
        //     image_out.get_height(),
        //     3,
        //     &image_out.get_rgb()[0],
        //     image_out.get_width() * 3);

        stbi_write_png("img_supertest.png", img.get_width(), img.get_height(), 3, img.get_rgb().data(), img.get_width() * 3);
        stbi_write_png("out.png", image_out.get_width(), image_out.get_height(), 3, image_out.get_rgb().data(), image_out.get_width() * 3);
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

    return EXIT_SUCCESS;
}