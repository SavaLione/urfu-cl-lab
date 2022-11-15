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

#include "core/settings.h"
#include "compute/compute_cpu.h"
#include "compute/compute_gpu.h"

#include <cstdlib>
#include <getopt.h>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <iostream>

int main(int argc, char *argv[])
{
	/* Signal handler */
	signal(SIGINT, signal_callback);

	/* Settings instance */
	settings &settings_instance = settings::instance();

	/* Options */
	std::string const short_opts = "gcv:i:h";
	std::array<option, 5> long_options =
	{{
		{"gpu-only", 			no_argument,        nullptr, 'g'},
		{"cpu-only", 			no_argument,        nullptr, 'c'},
		{"vector-size", 		required_argument,  nullptr, 'v'},
		{"iteration-count", 	required_argument,  nullptr, 'i'},
		{"help", 				no_argument,        nullptr, 'h'}
	}};

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
				int i = std::stoi(optarg);
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
			case 'h':
			case '?':
			default:
				print_help();
				break;
		}
	}

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

void print_help()
{
	std::cout << "Usage: nyx [OPTION]" << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -g, --gpu-only                  Perform only gpu tests" << std::endl;
	std::cout << "  -c, --cpu-only                  Perform only cpu tests" << std::endl;
	std::cout << "  -v, --vector-size <size>        Vector of elements size (default: 102400000)" << std::endl;
	std::cout << "  -i, --iteration-count <count>   Count of iterations (default: 100)" << std::endl;
	std::cout << "  -h, --help                      Display this help and exit" << std::endl;
	exit(EXIT_SUCCESS);
}