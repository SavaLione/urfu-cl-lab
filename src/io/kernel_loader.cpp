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
 * @brief OpenCL kernel loader
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Sep 2022
 */
#include "io/kernel_loader.h"

#include "io/log/logger.h"

#include <SDL2/SDL.h>
#include <cstddef>
#include <exception>

kernel_loader::kernel_loader() {}

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
            std::string file_name = "kernels/" + name + ".cl";

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

            _loaded_kernels.push_back(name);
            _string_kernels.push_back(kernel);

            spdlog::debug("Kernel {} loaded", name);
        }
        catch(std::exception const &e)
        {
            spdlog::error("Error trying to load kernel from file: {}", name);
            spdlog::error(e.what());
        }
        catch(...)
        {
            spdlog::error("Undefined error in kernel_loader");
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
    load("addition_lattice_2d");

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

    load("log_vector_16");
    load("log_vector_8");
    load("log_vector_4");
    load("log_vector_2");
}