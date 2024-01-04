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
 * @brief Compute on CPU
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Oct 2022
 */
#include "compute/compute_cpu.h"

#include "compute/fill_vectors.h"

compute_cpu::compute_cpu(std::size_t const &vector_size, std::size_t const &iteration_count)
{
    this->vector_size     = vector_size;
    this->iteration_count = iteration_count;
}

std::string compute_cpu::get_string_name(operation_name name)
{
    switch(name)
    {
        case ADDITION:
        {
            return "Addition";
            break;
        }
        case REMOVE:
        {
            return "Remove";
            break;
        }
        case MULTIPLE:
        {
            return "Multiple";
            break;
        }
        case DIVIDE:
        {
            return "Divide";
            break;
        }
        case EXPONENTIATION:
        {
            return "Exponentiation";
            break;
        }
        case LOG:
        {
            return "Log";
            break;
        }
        case UNKNOWN:
        {
            return "Unknown";
            break;
        }
        default:
        {
            return "UNKNOWN_OPERATION";
            break;
        }
    }

    return "UNKNOWN_OPERATION";
}

void compute_cpu::run_all()
{
    std::vector<float> vec_a(vector_size, 0);
    std::vector<float> vec_b(vector_size, 0);
    std::vector<float> vec_c(vector_size, 0);

    fill_vectors(vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end());

    _compute(operation_name::ADDITION, vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_c.begin(), vec_c.end());
    _compute(operation_name::REMOVE, vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_c.begin(), vec_c.end());
    _compute(operation_name::MULTIPLE, vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_c.begin(), vec_c.end());
    _compute(operation_name::DIVIDE, vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_c.begin(), vec_c.end());

    _compute(operation_name::EXPONENTIATION, vec_a.begin(), vec_a.end(), vec_c.begin(), vec_c.end());
    _compute(operation_name::LOG, vec_a.begin(), vec_a.end(), vec_c.begin(), vec_c.end());
}