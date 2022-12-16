/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Saveliy Pototskiy (SavaLione) (savalione.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/**
 * @file
 * @brief Nyx application
 * @author Saveliy Pototskiy (SavaLione)
 * @date 21 Sep 2022
 */
#include "core/nyx.h"

// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include "compute/compute_cpu.h"
#include "compute/compute_gpu.h"
#include "core/settings.h"
#include "platform/platform.h"
#include "io/log/logger.h"
#include "core/cl_image.h"
#include "core/cl_mandelbrot.h"
#include "core/sdl_wrapper.h"
#include "core/rgb_triangle.h"
#include "core/vao_triangle.h"

#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <stdexcept>

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

    /* Settings instance */
    settings &settings_instance = settings::instance();

    /* Options */
    std::string const short_opts = "gcv:i:l:bhu";

    std::array<option, 8> long_options = {
        {{"gpu-only", no_argument, nullptr, 'g'},
         {"cpu-only", no_argument, nullptr, 'c'},
         {"vector-size", required_argument, nullptr, 'v'},
         {"iteration-count", required_argument, nullptr, 'i'},
         {"laboratory-work", required_argument, nullptr, 'l'},
         {"verbose", no_argument, nullptr, 'b'},
         {"help", no_argument, nullptr, 'h'},
         {"build-info", no_argument, nullptr, 'u'}
         }};

    while(true)
    {
        auto const opt = getopt_long(argc, argv, short_opts.c_str(), long_options.data(), nullptr);

        if(opt == -1)
        {
            print_help();
            break;
        }

        switch(opt)
        {
            case 'g':
                settings_instance.set_gpu(true);
                settings_instance.set_cpu(false);
                spdlog::info("Perform only gpu tests");
                break;
            case 'c':
                settings_instance.set_gpu(false);
                settings_instance.set_cpu(true);
                spdlog::info("Perform only cpu tests");
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
                    spdlog::error("argument -v or --vector-size must be greater than 16, because we use OpenCV sixteen component vectors");
                    exit(EXIT_FAILURE);
                }

                if((v % 16) != 0)
                {
                    spdlog::error("argument -v or --vector-size must be multiple of 16, because we use OpenCV sixteen component vectors");
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
            case 'l':
            {
                int l = 0;
                try
                {
                    l = std::stoi(optarg);
                }
                catch(std::invalid_argument const &e)
                {
                    spdlog::error("unexpected -l or --laboratory-work argument: {}\n{}", optarg, e.what());
                    exit(EXIT_FAILURE);
                }
                catch(...)
                {
                    spdlog::error("unexpected -l or --laboratory-work argument: {}", optarg);
                    exit(EXIT_FAILURE);
                }

                switch(l)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        settings_instance.set_laboratory_work(l);
                        break;
                    default:
                        spdlog::error("argument -l or --laboratory-work must be 1, 2, 3, 4 or 5");
                        print_help();
                        break;
                }

                spdlog::info("Laboratory work number: {}", l);

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
            case '?':
            default:
                print_help();
                break;
        }
    }

    try
    {
        switch(settings_instance.get_laboratory_work())
        {
            case 1:
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
                break;
            }
            case 2:
            {
                cl_image cli;
                cli.run();
                break;
            }
            case 3:
            {
                cl_mandelbrot clm;
                clm.run();
                break;
            }
            case 4:
            {
                rgb_triangle rgb;
                rgb.run();
                break;
            }
            case 5:
            {
                vao_triangle vt;
                vt.run();
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
    std::cout << "  -g, --gpu-only                  Perform only gpu tests" << std::endl;
    std::cout << "  -c, --cpu-only                  Perform only cpu tests" << std::endl;
    std::cout << "  -v, --vector-size <size>        Vector of elements size (default: 102400000)" << std::endl;
    std::cout << "  -i, --iteration-count <count>   Count of iterations (default: 100)" << std::endl;
    std::cout << "  -l, --laboratory-work <number>  Laboratory work number (default: 1)" << std::endl;
    std::cout << "                                  --laboratory-work must be: 1, 2, 3, 4 or 5" << std::endl;
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