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
#ifndef COMPUTE_COMPUTE_CPU_H
#define COMPUTE_COMPUTE_CPU_H

#include "core/execution_time.h"

#include <exception>
#include <iterator>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <vector>

class compute_cpu
{
public:
	compute_cpu();
	~compute_cpu();

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
	void _compute(
		operation_name name,
		iterator_type start_iterator_a,
		iterator_type end_iterator_a,
		iterator_type start_iterator_c,
		iterator_type end_iterator_c)
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
						start_iterator_c[i] = log(start_iterator_a[i]);
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

	const std::size_t vector_size	  = 102400000;
	const std::size_t iteration_count = 100;
};

#endif // COMPUTE_COMPUTE_CPU_H