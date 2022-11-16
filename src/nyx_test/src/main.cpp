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

std::size_t IMAGE_WIDTH = 1024;
std::size_t IMAGE_HIGHT = 1024;

std::size_t BLOCK_SIZE = 32;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class image
{
public:
private:
    std::vector<cl::Platform> all_platforms;
    cl::Platform default_platform;
    std::vector<cl::Device> all_devices;
    cl::Device default_device;
    cl::Context context;
    cl::Program::Sources sources;
    std::vector<std::string> kernels;
    cl::Program program;

public:
    void read_kernel();
    void run();
};

void image::read_kernel()
{
    std::string kernel;
    std::string file_name = "example.cl";

    SDL_RWops *file = SDL_RWFromFile(file_name.c_str(), "rb");
    size_t size;

    if(!file)
    {
        throw std::runtime_error("Failed opening file: " + file_name);
    }

    void *loaded = SDL_LoadFile_RW(file, &size, 1);

    if(!loaded)
    {
        throw std::runtime_error("Failed loading file: " + file_name);
    }

    kernel = {static_cast<char *>(loaded), size};

    SDL_free(loaded);

    kernels.push_back(kernel);

    spdlog::info("Kernel {} loaded", file_name);
}

void image::run()
{
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
            return;
        }

        //////////////////////////////////////////////

        
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

/*
    We create (1024*1024) kernels
*/

struct pixel_direction
{
    /* int x; */               /* OpenCL kernel defined const */
    /* int y; */               /* OpenCL kernel defined const */
    /* unsigned char speed; */ /* OpenCL kernel source const */
    unsigned char power;
    unsigned char direction;
    /*
        Direction 0-8 (9)
            | | | |
            | |x| |
            | | | |
        
            |0|1|2|
            |3|4|5|
            |6|7|8|

            4 - unused  
    */
};

/*
const uchar SPEED = 1;

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void example(
    __read_only image2d_t img_in,
    __global __constant uchar* power,
    __global __constant uchar* direction,
    __write_only image2d_t img_out)
{
    int2 pos = {get_global_id(0), get_global_id(1)};

    uint4 pixel = read_imageui(img_in, sampler, pos);

    uint4 pixel_rdy = (int4)(0.0f);
    if((pos.x % 2) == 0)
    {
        pixel_rdy = pixel;
    }

    write_imagef (img_out, (int2)(pos.x, pos.y), pixel_rdy);
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    SDL_Window *window = SDL_CreateWindow("Laboratory work 2", 0, 0, IMAGE_WIDTH, IMAGE_HIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

    /* OpenCL */
    image _img;
    _img.read_kernel();
    _img.run();

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