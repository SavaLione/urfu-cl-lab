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
#ifndef COMPUTE_COMPUTE_CPU_H
#define COMPUTE_COMPUTE_CPU_H

#include "core/execution_time.h"
#include "io/log/logger.h"

#include <exception>
#include <string>
#include <vector>

class compute_cpu
{
public:
    compute_cpu(std::size_t const &vector_size, std::size_t const &iteration_count);

    void run_all();

    enum operation_name
    {
        ADDITION,
        REMOVE,
        MULTIPLE,
        DIVIDE,
        EXPONENTIATION,
        LOG,
        UNKNOWN
    };

private:
    std::string get_string_name(operation_name name);

    template<typename iterator_type>
    void _compute(
        operation_name name,
        iterator_type start_iterator_a,
        iterator_type end_iterator_a,
        iterator_type start_iterator_b,
        iterator_type end_iterator_b,
        iterator_type start_iterator_c,
        iterator_type end_iterator_c);

    template<typename iterator_type>
    void _compute(operation_name name, iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_c, iterator_type end_iterator_c);

    std::size_t vector_size     = 102400000;
    std::size_t iteration_count = 100;
};

///////////////////////////////////////////////////////////////////////////////

template<typename iterator_type>
void compute_cpu::_compute(
    operation_name name,
    iterator_type start_iterator_a,
    iterator_type end_iterator_a,
    iterator_type start_iterator_b,
    iterator_type end_iterator_b,
    iterator_type start_iterator_c,
    iterator_type end_iterator_c)
{
    typedef typename std::iterator_traits<iterator_type>::value_type data_type;
    typedef typename std::iterator_traits<iterator_type> data_size;

    std::size_t size_of_a = sizeof(data_type) * (end_iterator_a - start_iterator_a);
    std::size_t size_of_b = sizeof(data_type) * (end_iterator_b - start_iterator_b);
    std::size_t size_of_c = sizeof(data_type) * (end_iterator_c - start_iterator_c);

    std::size_t size_a = sizeof(data_size) * (end_iterator_a - start_iterator_a);
    std::size_t size_b = sizeof(data_size) * (end_iterator_b - start_iterator_b);
    std::size_t size_c = sizeof(data_size) * (end_iterator_c - start_iterator_c);

    if((size_of_a != size_of_b) && (size_of_a != size_of_c))
    {
        throw std::logic_error("Iterators are not equal.");
    }

    spdlog::info("Compute CPU application: {}", get_string_name(name));

    execution_time et;
    et.start();

    switch(name)
    {
        case ADDITION:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = start_iterator_a[i] + start_iterator_b[i];
                }
            }
            break;
        }
        case REMOVE:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = start_iterator_a[i] - start_iterator_b[i];
                }
            }
            break;
        }
        case MULTIPLE:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = start_iterator_a[i] * start_iterator_b[i];
                }
            }
            break;
        }
        case DIVIDE:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = start_iterator_a[i] / start_iterator_b[i];
                }
            }
            break;
        }
        default:
        {
            throw std::invalid_argument("Operation name type not found.");
            break;
        }
    }

    et.stop();

    spdlog::info("Time to parallel compute on cpu: {} (nanoseconds)", et.count_nanoseconds());
    spdlog::info("Time to parallel compute on cpu: {} (milliseconds)", et.count_milliseconds());
}

template<typename iterator_type>
void compute_cpu::_compute(operation_name name, iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_c, iterator_type end_iterator_c)
{
    typedef typename std::iterator_traits<iterator_type>::value_type data_type;
    typedef typename std::iterator_traits<iterator_type> data_size;

    std::size_t size_of_a = sizeof(data_type) * (end_iterator_a - start_iterator_a);
    std::size_t size_of_c = sizeof(data_type) * (end_iterator_c - start_iterator_c);

    std::size_t size_a = sizeof(data_size) * (end_iterator_a - start_iterator_a);
    std::size_t size_c = sizeof(data_size) * (end_iterator_c - start_iterator_c);

    if(size_of_a != size_of_c)
    {
        throw std::logic_error("Iterators are not equal.");
    }

    spdlog::info("Compute CPU application: {}", get_string_name(name));

    execution_time et;
    et.start();

    switch(name)
    {
        case EXPONENTIATION:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = start_iterator_a[i] * start_iterator_a[i];
                }
            }
            break;
        }
        case LOG:
        {
#pragma omp parallel for
            for(std::size_t ic = 0; ic < iteration_count; ic++)
            {
                for(std::size_t i = 0; i < size_c; i++)
                {
                    start_iterator_c[i] = std::log(start_iterator_a[i]);
                }
            }
            break;
        }
        default:
        {
            throw std::invalid_argument("Operation name type not found.");
            break;
        }
    }

    et.stop();

    spdlog::info("Time to parallel compute on cpu: {} (nanoseconds)", et.count_nanoseconds());
    spdlog::info("Time to parallel compute on cpu: {} (milliseconds)", et.count_milliseconds());
}

#endif // COMPUTE_COMPUTE_CPU_H