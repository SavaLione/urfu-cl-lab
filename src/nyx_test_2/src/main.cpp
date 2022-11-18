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

std::size_t IMAGE_SIZE_MAX = 16;
std::vector<uint8_t> IMAGE_ORIGINAL(IMAGE_SIZE_MAX *IMAGE_SIZE_MAX * 3);
std::vector<uint8_t> IMAGE_POSSIBLE_RESULT(IMAGE_SIZE_MAX *IMAGE_SIZE_MAX * 3);
std::size_t const IMAGE_VECTOR_SIZE = IMAGE_SIZE_MAX * IMAGE_SIZE_MAX * 3;

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

    std::string kernel = {"__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;"
                          ""
                          "__kernel void example("
                          "    __read_only image2d_t img_in,"
                          "    __write_only image2d_t img_out)"
                          "{"
                          "    const int x = get_global_id(0);"
                          "    const int y = get_global_id(1);"
                          //   "    float value = read_imagef(img_in, sampler, (int2)(x, y)).x;"
                          //   "    write_imagef(img_out, (int2)(x, y), value);"
                          "    int value = read_imagei(img_in, sampler, (int2)(x, y)).x;"
                          "    write_imagei(img_out, (int2)(x, y), value);"
                          "}"
                          ""
                          ""
                          ""
                          ""
                          ""
                          ""};

    kernels.push_back(kernel);

    for(std::size_t i = 0, fl = 0, color = 0; i < (IMAGE_SIZE_MAX * IMAGE_SIZE_MAX * 3); i++, fl++, color++)
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

    for(std::size_t i = 0, fl = 0; i < (IMAGE_SIZE_MAX * IMAGE_SIZE_MAX * 3); i++, fl++)
    {
        if(fl == 3)
            fl = 0;

        if(fl == 0)
            IMAGE_POSSIBLE_RESULT[i] = 128;
        if(fl == 1)
            IMAGE_POSSIBLE_RESULT[i] = 128;
        if(fl == 2)
            IMAGE_POSSIBLE_RESULT[i] = 255;
    }

    /////////////////////////////////////////
    int image_width    = 0;
    int image_height   = 0;
    int image_channels = 0;
    unsigned char *img = stbi_load("test.png", &image_width, &image_height, &image_channels, 0);

    if(img == NULL)
    {
        spdlog::error("Can't load test.png image");
        exit(EXIT_FAILURE);
    }

    spdlog::info("image_width:    {}", image_width);
    spdlog::info("image_height:   {}", image_height);
    spdlog::info("image_channels: {}", image_channels);
    spdlog::info("img size:       {}", sizeof(img));

    std::vector<uint8_t> img_out(image_width * image_height * image_channels);

    int index = 0;
    for(int j = image_height - 1; j >= 0; --j)
    {
        for(int i = 0; i < image_width; ++i)
        {
            int ind_0      = index++;
            int ind_1      = index++;
            int ind_2      = index++;
            img_out[ind_0] = img[ind_0];
            img_out[ind_1] = img[ind_1];
            img_out[ind_2] = img[ind_2];
        }
    }

    stbi_write_png("test_out.png", image_width, image_height, image_channels, img_out.data(), image_width * image_channels);

    stbi_image_free(img);

    //////////////////////////////////////////////
    stbi_write_png("IMAGE_ORIGINAL.png", IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 3, IMAGE_ORIGINAL.data(), IMAGE_SIZE_MAX * 3);
    // stbi_write_png("IMAGE_POSSIBLE_RESULT.png", IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 3, IMAGE_POSSIBLE_RESULT.data(), IMAGE_SIZE_MAX * 3);
    //////////////////////////////////////////////

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

        cl::CommandQueue queue(context, default_device);

        cl::Kernel kernel_simple_add(program, "example");
        cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);

        uint8_t data_image[IMAGE_VECTOR_SIZE];
        uint8_t data_image_out[IMAGE_VECTOR_SIZE];

        for(std::size_t i = 0; i < IMAGE_VECTOR_SIZE; i++)
        {
            data_image[i] = IMAGE_ORIGINAL[i];
        }

        cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 0, &data_image[0]);
        cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 0, NULL);

        cl::NDRange global(IMAGE_SIZE_MAX, IMAGE_SIZE_MAX);
        std::array<cl::size_type, 3> origin {0, 0, 0};
        std::array<cl::size_type, 3> region {16, 16, 1};

        kernel_simple_add.setArg(0, img_2d_in);
        kernel_simple_add.setArg(1, img_2d_out);

        queue.enqueueNDRangeKernel(kernel_simple_add, cl::NullRange, global, cl::NullRange);
        queue.finish();

        queue.enqueueReadImage(img_2d_out, CL_TRUE, origin, region, 0, 0, data_image_out);

        std::vector<uint8_t> img_out_new(IMAGE_VECTOR_SIZE);

        for(std::size_t i = 0; i < IMAGE_VECTOR_SIZE; i++)
        {
            img_out_new[i] = data_image[i];
        }

        spdlog::info("img_out_new.size() {}", img_out_new.size());
        spdlog::info("    {}", img_out_new[0]);
        spdlog::info("    {}", img_out_new[1]);
        spdlog::info("    {}", img_out_new[2]);
        spdlog::info("    {}", img_out_new[3]);
        spdlog::info("IMAGE_ORIGINAL.size() {}", IMAGE_ORIGINAL.size());
        spdlog::info("    {}", IMAGE_ORIGINAL[0]);
        spdlog::info("    {}", IMAGE_ORIGINAL[1]);
        spdlog::info("    {}", IMAGE_ORIGINAL[2]);
        spdlog::info("    {}", IMAGE_ORIGINAL[3]);
        stbi_write_png("IMAGE_POSSIBLE_RESULT.png", IMAGE_SIZE_MAX, IMAGE_SIZE_MAX, 3, img_out_new.data(), IMAGE_SIZE_MAX * 3);
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