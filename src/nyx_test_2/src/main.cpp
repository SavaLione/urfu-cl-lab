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
                          "    pixel.x += 50;"
                          "    pixel.y += 50;"
                          "    pixel.z += 50;"
                          "    write_imageui(img_out, pos, pixel);"
                          "}"
                          ""
                          ""};

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
            return EXIT_FAILURE;
        }

        cl::CommandQueue queue(context, default_device, 0, NULL);

        cl::Kernel kernel_simple_add(program, "example");
        cl::ImageFormat format(CL_RGBA, CL_UNSIGNED_INT8);

        // uint8_t data_image_in[img.size() * 4];
        // uint8_t data_image_out[img.size() * 4];
        auto data_image_in  = new uint8_t[img.size() * 4];
        auto data_image_out = new uint8_t[img.size() * 4];
        // std::vector<uint8_t> data_image_in = img.get_rgba();
        // std::vector<uint8_t> data_image_out(img.get_size_rgba());
        // std::vector<uint8_t> data_image_in = img.get_rgba();
        // std::vector<uint8_t> data_image_out;
        // data_image_out.resize(img.size() * 4);

        //std::copy(img.get_rgba().begin(), img.get_rgba().end(), data_image_in);
        for(std::size_t i = 0, fl = 0; i < img.size(); i++, fl +=4)
        {
            data_image_in[fl + 0] = img.get()[i].r;
            data_image_in[fl + 1] = img.get()[i].g;
            data_image_in[fl + 2] = img.get()[i].b;
            data_image_in[fl + 3] = img.get()[i].a;
        }

        cl::Image2D img_2d_in(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, img.get_width(), img.get_height(), 0, &data_image_in[0]);
        cl::Image2D img_2d_out(context, CL_MEM_WRITE_ONLY, format, img.get_width(), img.get_height(), 0, NULL);

        cl::NDRange global(img.get_width(), img.get_height());
        // std::array<cl::size_type, 3> origin {0, 0, 0};
        // std::array<cl::size_type, 3> region {16, 16, 1};
        std::array<cl::size_type, 3> origin;
        origin[0] = 0;
        origin[1] = 0;
        origin[2] = 0;
        std::array<cl::size_type, 3> region;
        // region[0] = 16;
        // region[1] = 16;
        // region[2] = 1;
        region[0] = img.get_width();
        region[1] = img.get_height();
        region[2] = 1;
        // cl::size_type origin[3] = {0, 0, 0};
        // cl::size_type region[3] = {16, 16, 1};

        kernel_simple_add.setArg(0, img_2d_in);
        kernel_simple_add.setArg(1, img_2d_out);

        queue.enqueueNDRangeKernel(kernel_simple_add, cl::NullRange, global, cl::NullRange);
        queue.finish();

        queue.enqueueReadImage(img_2d_out, CL_TRUE, origin, region, 0, 0, &data_image_out[0]);

        image image_out(img.get_width(), img.get_height(), img.get_channels(), image::IMAGE_TYPE::RGBA, &data_image_out[0]);

        //stbi_write_png("img_supertest.png", img.get_width(), img.get_height(), 3, img.get_rgb().data(), img.get_width() * 3);
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

    spdlog::info("exit");

    return EXIT_SUCCESS;
}