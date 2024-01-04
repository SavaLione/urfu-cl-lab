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
 * @brief Application settings
 * @author Savelii Pototskii (savalione.com)
 * @date 15 Nov 2022
 */
#include "core/settings.h"

settings::settings() {}

bool settings::get_gpu()
{
    return gpu;
}

bool settings::get_cpu()
{
    return cpu;
}

std::size_t settings::get_vector_size()
{
    return vector_size;
}

std::size_t settings::get_iteration_count()
{
    return iteration_count;
}

void settings::set_gpu(bool const &gpu)
{
    this->gpu = gpu;
}

void settings::set_cpu(bool const &cpu)
{
    this->cpu = cpu;
}

void settings::set_vector_size(std::size_t const &vector_size)
{
    this->vector_size = vector_size;
}

void settings::set_iteration_count(std::size_t const &iteration_count)
{
    this->iteration_count = iteration_count;
}

std::size_t settings::get_laboratory_work()
{
    return laboratory_work;
}

void settings::set_laboratory_work(std::size_t const &laboratory_work)
{
    this->laboratory_work = laboratory_work;
}

bool settings::get_verbose()
{
    return verbose;
}

void settings::set_verbose(bool const &verbose)
{
    this->verbose = verbose;
}

bool settings::get_exit()
{
    return exit;
}

void settings::set_exit(bool const &exit)
{
    this->exit = exit;
}