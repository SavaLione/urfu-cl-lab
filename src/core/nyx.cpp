/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2022-2024 Savelii Pototskii (savalione.com)
 * All rights reserved.
 *
 * Author: Savelii Pototskii <monologuesplus@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file
 * @brief Nyx application
 * @author Savelii Pototskii (savalione.com)
 * @date 21 Sep 2022
 */
#include "core/nyx.h"

// clang-format off
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <CL/cl_gl.h>
// clang-format on

#include "compute/compute_cpu.h"
#include "compute/compute_gpu.h"
#include "core/settings.h"
#include "platform/platform.h"
#include "io/log/logger.h"
#include "gui/cl_image.h"
#include "gui/cl_mandelbrot.h"
#include "gui/sdl_wrapper.h"
#include "gui/rgb_triangle.h"
#include "gui/vao_triangle.h"
#include "gui/gl_matrices.h"
#include "gui/interop.h"
#include "gui/cl_particles.h"
#include "gui/rgb_cube.h"
#include "gui/rgb_cube_texture.h"

#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

int main(int argc, char *argv[])
{
    /* Print help if used didn't write any flag */
    if(argc <= 1)
        print_help();

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

    /* Settings instance */
    settings &settings_instance = settings::instance();

    /* Options */
    std::string const short_opts = "gcv:i:t:bhu";

    std::array<option, 8> long_options = {
        {{"gpu", no_argument, nullptr, 'g'},
         {"cpu", no_argument, nullptr, 'c'},
         {"vector-size", required_argument, nullptr, 'v'},
         {"iteration-count", required_argument, nullptr, 'i'},
         {"task-number", required_argument, nullptr, 't'},
         {"verbose", no_argument, nullptr, 'b'},
         {"help", no_argument, nullptr, 'h'},
         {"build-info", no_argument, nullptr, 'u'}}};

    while(true)
    {
        auto const opt = getopt_long(argc, argv, short_opts.c_str(), long_options.data(), nullptr);

        if(opt == -1)
        {
            break;
        }

        switch(opt)
        {
            case 'g':
                settings_instance.set_gpu(true);
                spdlog::info("Perform gpu tests");
                break;
            case 'c':
                settings_instance.set_cpu(true);
                spdlog::info("Perform cpu tests");
                break;
            case 'v':
            {
                int v = 0;
                try
                {
                    v = std::stoi(optarg);
                }
                catch(std::invalid_argument const &e)
                {
                    spdlog::error("unexpected -v or --vector-size argument: {}\n{}", optarg, e.what());
                    exit(EXIT_FAILURE);
                }
                catch(...)
                {
                    spdlog::error("unexpected -v or --vector-size argument: {}", optarg);
                    exit(EXIT_FAILURE);
                }

                if(v <= 0)
                {
                    spdlog::error("argument -v or --vector-size must be greater than zero");
                    exit(EXIT_FAILURE);
                }

                if(v < 16)
                {
                    spdlog::error("argument -v or --vector-size must be greater than 16, because we use OpenCL sixteen component vectors");
                    exit(EXIT_FAILURE);
                }

                if((v % 16) != 0)
                {
                    spdlog::error("argument -v or --vector-size must be multiple of 16, because we use OpenCL sixteen component vectors");
                    exit(EXIT_FAILURE);
                }

                spdlog::info("Vector of elements size: {}", v);

                settings_instance.set_vector_size(v);
                break;
            }
            case 'i':
            {
                int i = 0;
                try
                {
                    i = std::stoi(optarg);
                }
                catch(std::invalid_argument const &e)
                {
                    spdlog::error("unexpected -i or --iteration-count argument: {}\n{}", optarg, e.what());
                    exit(EXIT_FAILURE);
                }
                catch(...)
                {
                    spdlog::error("unexpected -i or --iteration-count argument: {}", optarg);
                    exit(EXIT_FAILURE);
                }

                if(i <= 0)
                {
                    spdlog::error("argument -i or --iteration-count must be greater than zero");
                    exit(EXIT_FAILURE);
                }

                spdlog::info("Count of iterations: {}", i);

                settings_instance.set_iteration_count(i);
                break;
            }
            case 't':
            {
                int t = 0;
                try
                {
                    t = std::stoi(optarg);
                }
                catch(std::invalid_argument const &e)
                {
                    spdlog::error("unexpected -t or --task-number argument: {}\n{}", optarg, e.what());
                    exit(EXIT_FAILURE);
                }
                catch(...)
                {
                    spdlog::error("unexpected -t or --task-number argument: {}", optarg);
                    exit(EXIT_FAILURE);
                }

                switch(t)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        settings_instance.set_laboratory_work(t);
                        break;
                    default:
                        spdlog::error("argument -t or --task-number must be 1, 2, 3, 4, 5, 6, 7, 8 or 9");
                        print_help();
                        break;
                }

                spdlog::info("Task number: {}", t);

                break;
            }
            case 'b':
                settings_instance.set_verbose(true);
                spdlog::info("Verbose output set");
                spdlog::set_level(spdlog::level::debug);
                spdlog::debug("Verbose output example");
                break;
            case 'u':
                print_build_info();
                break;
            case 'h':
            default:
                print_help();
                break;
        }
    }

    try
    {
        /* Kernel loader instance */
        if(settings_instance.get_gpu())
        {
            kernel_loader &kernel_loader_instance = kernel_loader::instance();
            kernel_loader_instance.load();
        }

        /* Compute the test data on cpu */
        if(settings_instance.get_cpu())
        {
            compute_cpu cc(settings_instance.get_vector_size(), settings_instance.get_iteration_count());
            cc.run_all();
        }

        /* Compute the test data on gpu */
        if(settings_instance.get_gpu())
        {
            compute_gpu cg(settings_instance.get_vector_size(), settings_instance.get_iteration_count());
            cg.print_info();
            cg.run_all();
        }
    }
    catch(std::exception const &e)
    {
        spdlog::error(e.what());
        exit(EXIT_FAILURE);
    }
    catch(...)
    {
        spdlog::error("Unexpected error");
        exit(EXIT_FAILURE);
    }

    if(settings_instance.get_laboratory_work() != 0)
        try
        {
            switch(settings_instance.get_laboratory_work())
            {
                case 1:
                {
                    cl_image cli;
                    cli.run();
                    break;
                }
                case 2:
                {
                    cl_mandelbrot clm;
                    clm.run();
                    break;
                }
                case 3:
                {
                    rgb_triangle rgb;
                    rgb.run();
                    break;
                }
                case 4:
                {
                    vao_triangle vt;
                    vt.run();
                    break;
                }
                case 5:
                {
                    gl_matrices glm;
                    glm.run();
                    break;
                }
                case 6:
                {
                    interop iop;
                    iop.run();
                    break;
                }
                case 7:
                {
                    cl_particles clp;
                    clp.run();
                    break;
                }
                case 8:
                {
                    rgb_cube rc;
                    rc.run();
                    break;
                }
                case 9:
                {
                    rgb_cube_texture rct;
                    rct.run();
                    break;
                }
                default:
                    print_help();
                    break;
            }
        }
        catch(std::exception const &e)
        {
            spdlog::error(e.what());
            exit(EXIT_FAILURE);
        }
        catch(...)
        {
            spdlog::error("Unexpected error");
            exit(EXIT_FAILURE);
        }

    return EXIT_SUCCESS;
}

void print_help()
{
    std::cout << "Usage: nyx [OPTION]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -g, --gpu                       Perform gpu tests (on gpu via OpenCL)" << std::endl;
    std::cout << "  -c, --cpu                       Perform cpu tests (on cpu)" << std::endl;
    std::cout << "  -v, --vector-size <size>        Vector of elements size (default: 102400000)" << std::endl;
    std::cout << "  -i, --iteration-count <count>   Count of iterations (default: 100)" << std::endl;
    std::cout << "  -t, --task-number <number>      Task number" << std::endl;
    std::cout << "                                  --task-number must be: 1, 2, 3, 4, 5, 6, 7, or 8 where:" << std::endl;
    std::cout << "                                      1 - draw some buffer via OpenCL buffer (deprecated)" << std::endl;
    std::cout << "                                      2 - draw Mandelbrot set via OpenCL buffer" << std::endl;
    std::cout << "                                      3 - draw OpenGL RGB triangle" << std::endl;
    std::cout << "                                      4 - draw OpenGL VAO triangle" << std::endl;
    std::cout << "                                      5 - draw OpenGL VAO triangle with matrices support" << std::endl;
    std::cout << "                                      6 - draw Mandelbrot set via OpenGL with interoperability" << std::endl;
    std::cout << "                                      7 - draw OpenCL particles with interoperability" << std::endl;
    std::cout << "                                      8 - draw OpenGL RGB cube" << std::endl;
    std::cout << "                                      9 - draw OpenGL RGB textured cube" << std::endl;
    std::cout << "  -b, --verbose                   Verbose output" << std::endl;
    std::cout << "  -h, --help                      Display help information and exit" << std::endl;
    std::cout << "  -u, --build-info                Display build information end exit" << std::endl;
    exit(EXIT_SUCCESS);
}

void print_build_info()
{
    std::cout << "Version: " << NYX_VERSION << std::endl;
    std::cout << "  Major: " << NYX_VERSION_MAJOR << std::endl;
    std::cout << "  Minor: " << NYX_VERSION_MINOR << std::endl;
    std::cout << "  Patch: " << NYX_VERSION_PATCH << std::endl;
    std::cout << std::endl;
    std::cout << "Description: " << NYX_DESCRIPTION << std::endl;
    std::cout << "Homepage url: " << NYX_HOMEPAGE_URL << std::endl;
    std::cout << std::endl;
    std::cout << "Project generator: " << NYX_GENERATOR << std::endl;
    std::cout << "Project build type: " << NYX_BUILD_TYPE << std::endl;
    std::cout << std::endl;
    std::cout << "Compile version: " << compiler_version() << std::endl;
    std::cout << "Compile date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;
    std::cout << "Project C compiler variables" << std::endl;
    std::cout << "    Extensions: " << NYX_C_EXTENSIONS << std::endl;
    std::cout << "    Standard: " << NYX_C_STANDARD << std::endl;
    std::cout << "    Compiler: " << NYX_C_COMPILER << std::endl;
    std::cout << "    Compiler id: " << NYX_C_COMPILER_ID << std::endl;
    std::cout << "    Compiler predefines command: " << NYX_C_COMPILER_PREDEFINES_COMMAND << std::endl;
    std::cout << "    Compiler target: " << NYX_C_COMPILER_TARGET << std::endl;
    std::cout << "    Compiler version: " << NYX_C_COMPILER_VERSION << std::endl;
    std::cout << "    Flags: " << NYX_C_FLAGS << std::endl;
    std::cout << "    Simulate id: " << NYX_C_SIMULATE_ID << std::endl;
    std::cout << "    Standard include derictories: " << NYX_C_STANDARD_INCLUDE_DIRECTORIES << std::endl;
    std::cout << "    Standard libraries: " << NYX_C_STANDARD_LIBRARIES << std::endl;
    std::cout << "    Standard required: " << NYX_C_STANDARD_REQUIRED << std::endl;
    std::cout << "    Compiler ABI: " << NYX_C_COMPILER_ABI << std::endl;
    std::cout << "    Compiler architecture id: " << NYX_C_COMPILER_ARCHITECTURE_ID << std::endl;
    std::cout << "    Compiler compiler version internal: " << NYX_C_COMPILER_VERSION_INTERNAL << std::endl;
    std::cout << "    Linker preference: " << NYX_C_LINKER_PREFERENCE << std::endl;
    std::cout << "    Linker preference propagates: " << NYX_C_LINKER_PREFERENCE_PROPAGATES << std::endl;
    std::cout << "    Platform id: " << NYX_C_PLATFORM_ID << std::endl;
    std::cout << "Project C++ compiler variables" << std::endl;
    std::cout << "    Extensions: " << NYX_CXX_EXTENSIONS << std::endl;
    std::cout << "    Standard: " << NYX_CXX_STANDARD << std::endl;
    std::cout << "    Compiler: " << NYX_CXX_COMPILER << std::endl;
    std::cout << "    Compiler id: " << NYX_CXX_COMPILER_ID << std::endl;
    std::cout << "    Compiler predefines command: " << NYX_CXX_COMPILER_PREDEFINES_COMMAND << std::endl;
    std::cout << "    Compiler target: " << NYX_CXX_COMPILER_TARGET << std::endl;
    std::cout << "    Compiler version: " << NYX_CXX_COMPILER_VERSION << std::endl;
    std::cout << "    Flags: " << NYX_CXX_FLAGS << std::endl;
    std::cout << "    Simulate id: " << NYX_CXX_SIMULATE_ID << std::endl;
    std::cout << "    Standard include derictories: " << NYX_CXX_STANDARD_INCLUDE_DIRECTORIES << std::endl;
    std::cout << "    Standard libraries: " << NYX_CXX_STANDARD_LIBRARIES << std::endl;
    std::cout << "    Standard required: " << NYX_CXX_STANDARD_REQUIRED << std::endl;
    std::cout << "    Compiler ABI: " << NYX_CXX_COMPILER_ABI << std::endl;
    std::cout << "    Compiler architecture id: " << NYX_CXX_COMPILER_ARCHITECTURE_ID << std::endl;
    std::cout << "    Compiler compiler version internal: " << NYX_CXX_COMPILER_VERSION_INTERNAL << std::endl;
    std::cout << "    Linker preference: " << NYX_CXX_LINKER_PREFERENCE << std::endl;
    std::cout << "    Linker preference propagates: " << NYX_CXX_LINKER_PREFERENCE_PROPAGATES << std::endl;
    std::cout << "    Platform id: " << NYX_CXX_PLATFORM_ID << std::endl;
    std::cout << std::endl;
    std::cout << "CMake version: " << NYX_CMAKE_VERSION << std::endl;
    std::cout << "        Major: " << NYX_CMAKE_VERSION_MAJOR << std::endl;
    std::cout << "        Minor: " << NYX_CMAKE_VERSION_MINOR << std::endl;
    std::cout << "        Patch: " << NYX_CMAKE_VERSION_PATCH << std::endl;
    std::cout << std::endl;
    std::cout << "Project options" << std::endl;
    std::cout << "    External fmt library: ";
#ifdef NYX_EXTERNAL_FMT
    std::cout << "YES";
#else
    std::cout << "YES";
#endif
    std::cout << std::endl;

    exit(EXIT_SUCCESS);
}