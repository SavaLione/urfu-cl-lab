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
 * @brief Compute on CPU
 * @author Saveliy Pototskiy (SavaLione)
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