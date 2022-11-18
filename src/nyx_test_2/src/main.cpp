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

std::size_t const IMAGE_SIZE_MAX         = 16;
std::size_t const IMAGE_PIXELS           = IMAGE_SIZE_MAX * IMAGE_SIZE_MAX;
std::size_t const IMAGE_VECTOR_SIZE_RGB  = IMAGE_SIZE_MAX * IMAGE_SIZE_MAX * 3;
std::size_t const IMAGE_VECTOR_SIZE_RGBA = IMAGE_SIZE_MAX * IMAGE_SIZE_MAX * 4;
std::vector<uint8_t> IMAGE_ORIGINAL(IMAGE_VECTOR_SIZE_RGB);

// union pixel_rgba
// {
//     uint32_t word;
//     uint8_t byte[4];
// };

// union pixel_rgb
// {
//     uint32_t word;
//     uint8_t byte[3];
// };

// class image
// {
// public:
//     image(std::size_t width, std::size_t height, std::vector<uint8_t> const &image) : _width(width), _height(height), _image(_width * _height)
//     {
//         if(image.size() != (width * height))
//             throw std::runtime_error("ERROR");

//         for(std::size_t i = 0; i < image.size(); i += 3)
//         {
//             pixel_rgba pixel;
//             pixel.byte[0] = image[i + 0];
//             pixel.byte[1] = image[i + 1];
//             pixel.byte[2] = image[i + 2];
//             pixel.byte[3] = 255;
//             _image.push_back(pixel);
//         }
//     };

//     image(std::size_t width, std::size_t height) : _width(width), _height(height) {}

//     void set_rgb(std::vector<uint8_t> const &image)
//     {
//         if(image.size() != (_width * _height))
//             throw std::runtime_error("ERROR");
//         _image.clear();

//         for(std::size_t i = 0; i < image.size(); i += 3)
//         {
//             pixel_rgba pixel;
//             pixel.byte[0] = image[i + 0];
//             pixel.byte[1] = image[i + 1];
//             pixel.byte[2] = image[i + 2];
//             pixel.byte[3] = 255;
//             _image.push_back(pixel);
//         }
//     }

//     void set_rgba(std::vector<uint8_t> const &image)
//     {
//         if(image.size() != (_width * _height))
//             throw std::runtime_error("ERROR");
//         _image.clear();

//         for(std::size_t i = 0; i < image.size(); i += 4)
//         {
//             pixel_rgba pixel;
//             pixel.byte[0] = image[i + 0];
//             pixel.byte[1] = image[i + 1];
//             pixel.byte[2] = image[i + 2];
//             pixel.byte[3] = image[i + 3];
//             _image.push_back(pixel);
//         }
//     }

//     std::vector<pixel_rgba> rgba()
//     {
//         return _image;
//     }

//     std::vector<pixel_rgb> rgb()
//     {
//         std::vector<pixel_rgb> ret;
//         for(std::size_t i = 0; i < _image.size(); i++)
//         {
//             pixel_rgb pixel;
//             pixel.byte[0] = _image[0].byte[0];
//             pixel.byte[1] = _image[0].byte[1];
//             pixel.byte[2] = _image[0].byte[2];
//             ret.push_back(pixel);
//         }
//         return ret;
//     }

//     std::size_t size()
//     {
//         return _width * _height;
//     }

// private:
//     std::size_t _width;
//     std::size_t _height;
//     std::vector<pixel_rgba> _image;
// };

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
                          "    pixel.x += 1;"
                          "    pixel.y += 2;"
                          "    write_imageui(img_out, pos, pixel);"
                          "}"
                          ""
                          ""};

    kernels.push_back(kernel);

    for(std::size_t i = 0, fl = 0, color = 0; i < (IMAGE_VECTOR_SIZE_RGB); i++, fl++, color++)
    {
        if(fl == 3)
            fl = 0;
        if(color == 256)
            color = 0;

        if(fl == 0)
            IMAGE_ORIGINAL[i] = color;
        if(fl == 1)
            IMAGE_ORIGINAL[i] = color;
        if(fl == 2)
            IMAGE_ORIGINAL[i] = 255;
    }

    try
    {
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

        //kernels = kernel_loader_instance.get();

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

        uint8_t data_image[IMAGE_VECTOR_SIZE_RGBA];
        //uint8_t data_image_out[IMAGE_VECTOR_SIZE_RGBA];

        // for(std::size_t i = 0; i < IMAGE_PIXELS; i += 4)
        // {
        //     data_image[i + 0] = IMAGE_ORIGINAL[i + 0];
        //     data_image[i + 1] = IMAGE_ORIGINAL[i + 1];
        //     data_image[i + 2] = IMAGE_ORIGINAL[i + 2];
        //     data_image[i + 3] = 255;
        // }
        // for(std::size_t i = 0; i < IMAGE_VECTOR_SIZE_RGBA; i++)
        // {
        //     if(i == 255)
        //         i = 0;
        //     data_image[i] = i;
        // }
        for(std::size_t i = 0; i < IMAGE_VECTOR_SIZE_RGBA; i++)
        {
            data_image[i] = 128;
        }

        cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 0, &data_image[0]);
        cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 0, NULL);

        cl::NDRange global(IMAGE_SIZE_MAX, IMAGE_SIZE_MAX);
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

        auto data_image_out = new unsigned char[IMAGE_VECTOR_SIZE_RGBA];

        queue.enqueueReadImage(img_2d_out, CL_TRUE, origin, region, 0, 0, &data_image_out[0]);

        std::vector<uint8_t> img_out_new(IMAGE_PIXELS);
        // for(std::size_t i = 0; i < IMAGE_PIXELS; i += 4)
        // {
        //     img_out_new.push_back(data_image_out[i + 0]);
        //     img_out_new.push_back(data_image_out[i + 1]);
        //     img_out_new.push_back(data_image_out[i + 2]);
        // }

        spdlog::info("data_image");
        spdlog::info("    {}", data_image[0]);
        spdlog::info("    {}", data_image[1]);
        spdlog::info("    {}", data_image[2]);
        spdlog::info("    {}", data_image[3]);
        spdlog::info("    {}", data_image[4]);
        spdlog::info("    {}", data_image[5]);
        spdlog::info("    {}", data_image[6]);
        spdlog::info("    {}", data_image[7]);
        spdlog::info("data_image_out");
        spdlog::info("    {}", data_image_out[0]);
        spdlog::info("    {}", data_image_out[1]);
        spdlog::info("    {}", data_image_out[2]);
        spdlog::info("    {}", data_image_out[3]);
        spdlog::info("    {}", data_image_out[4]);
        spdlog::info("    {}", data_image_out[5]);
        spdlog::info("    {}", data_image_out[6]);
        spdlog::info("    {}", data_image_out[7]);
        // spdlog::info("-----------------------------------------");
        // spdlog::info("img_out_new.size() {}", img_out_new.size());
        // spdlog::info("    {}", img_out_new[0]);
        // spdlog::info("    {}", img_out_new[1]);
        // spdlog::info("    {}", img_out_new[2]);
        // spdlog::info("    {}", img_out_new[3]);
        // spdlog::info("    {}", img_out_new[4]);
        // spdlog::info("    {}", img_out_new[5]);
        // spdlog::info("    {}", img_out_new[6]);
        // spdlog::info("    {}", img_out_new[7]);
        // spdlog::info("    {}", img_out_new[8]);
        // spdlog::info("    {}", img_out_new[9]);
        // spdlog::info("    {}", img_out_new[10]);
        // spdlog::info("    {}", img_out_new[11]);
        // spdlog::info("IMAGE_ORIGINAL.size() {}", IMAGE_ORIGINAL.size());
        // spdlog::info("    {}", IMAGE_ORIGINAL[0]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[1]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[2]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[3]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[4]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[5]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[6]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[7]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[8]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[9]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[10]);
        // spdlog::info("    {}", IMAGE_ORIGINAL[11]);

        // stbi_write_png("IMAGE_POSSIBLE_RESULT.png", IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 3, img_out_new.data(), IMAGE_SIZE_MAX * 3);
        // stbi_write_png("IMAGE_ORIGINAL.png", IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 3, IMAGE_ORIGINAL.data(), IMAGE_SIZE_MAX * 3);

        delete[] data_image_out;
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