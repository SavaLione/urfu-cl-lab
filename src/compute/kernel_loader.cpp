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
 * @brief OpenCL kernel loader
 * @author Saveliy Pototskiy (SavaLione)
 * @date 26 Sep 2022
 */
#include "compute/kernel_loader.h"

#include <cstddef>
#include <exception>
#include <fstream>
#include <iterator>
#include <spdlog/spdlog.h>
#include <sstream>

kernel_loader::kernel_loader() {}

kernel_loader::~kernel_loader() {}

void kernel_loader::load(std::string const &name)
{
	bool is_kernel_already_loaded = false;
	for(auto const &lk : _loaded_kernels)
	{
		if(lk == name)
		{
			is_kernel_already_loaded = true;
			spdlog::warn("Kernel {} already loaded.", name);
		}
	}

	if(!is_kernel_already_loaded)
	{
		try
		{
			std::string kernel;
			std::string path = "kernels/";
			path += name;
			path += ".cl";

			std::ifstream f(path);

			if(f.is_open())
			{
			}
			else
			{
				spdlog::error("Can't open: {}", path);
				f.close();
				return;
			}

			while(!f.eof())
			{
				std::string line = "";
				f >> line;
				line += " ";
				kernel += line;
			}

			f.close();

			_loaded_kernels.push_back(name);
			_string_kernels.push_back(kernel);

			spdlog::info("Kernel {} loaded.", name);
		}
		catch(std::exception const &e)
		{
			spdlog::error("Error trying to load kernel from file: {}", name);
			spdlog::error(e.what());
		}
		catch(std::ifstream::failure const &e)
		{
			spdlog::error("Error trying to load kernel from file: {}", name);
			spdlog::error(e.what());
		}
	}
}

void kernel_loader::print()
{
	for(std::size_t i = 0; (i < _loaded_kernels.size()) && (i < _string_kernels.size()); i++)
	{
		spdlog::info("Kernel: {} data: \n{}", _loaded_kernels[i], _string_kernels[i]);
	}
}

void kernel_loader::reset()
{
	_loaded_kernels.clear();
	_string_kernels.clear();
}

void kernel_loader::reload()
{
	reset();
	load();
}

void kernel_loader::load()
{
	load("simple_add");
	load("simple_divide");
	load("simple_exponentiation");
	load("simple_multiple");
	load("simple_remove");
	load("simple_vec_add");

	load("addition_vector_16");
	load("addition_vector_8");
	load("addition_vector_4");
	load("addition_vector_2");

	load("divide_vector_16");
	load("divide_vector_8");
	load("divide_vector_4");
	load("divide_vector_2");

	load("exponentiation_vector_16");
	load("exponentiation_vector_8");
	load("exponentiation_vector_4");
	load("exponentiation_vector_2");

	load("multiple_vector_16");
	load("multiple_vector_8");
	load("multiple_vector_4");
	load("multiple_vector_2");

	load("remove_vector_16");
	load("remove_vector_8");
	load("remove_vector_4");
	load("remove_vector_2");
}