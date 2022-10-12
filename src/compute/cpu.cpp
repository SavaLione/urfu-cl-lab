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
 * @brief CPU
 * @author Saveliy Pototskiy (SavaLione)
 * @date 12 Oct 2022
 */
#include "compute/cpu.h"

#include "core/execution_time.h"

#include <spdlog/spdlog.h>
#include <string>
#include <vector>

void cpu_task()
{
    spdlog::info("GPU task started.");
	const std::size_t vector_size	  = 102400000;
	const std::size_t iteration_count = 100;

	std::vector<float> vec_a(vector_size, 0);
	std::vector<float> vec_b(vector_size, 0);
	std::vector<float> vec_c(vector_size, 0);

	spdlog::info("vec_a size: {}", vec_a.size());
	spdlog::info("vec_b size: {}", vec_b.size());
	spdlog::info("vec_c size: {}", vec_c.size());

	execution_time et;
	et.start();

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = (float)i / 2;
	}

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_b.size(); i++)
	{
		vec_b[i] = (float)i * 3;
	}

	et.stop();

	spdlog::info("Time to parallel fill vectors: {} (nanoseconds)", et.count_nanoseconds());
	spdlog::info("Time to parallel fill vectors: {} (milliseconds)", et.count_milliseconds());

	et.start();

	for(std::size_t n = 0; n < iteration_count; n++)
	{
		for(std::size_t i = 0; i < vec_c.size(); i++)
		{
			vec_c[i] = vec_a[i] + vec_b[i];
		}
	}

	et.stop();

	spdlog::info("Time to compute vec_c on cpu: {} (nanoseconds)", et.count_nanoseconds());
	spdlog::info("Time to compute vec_c on cpu: {} (milliseconds)", et.count_milliseconds());

	et.start();

#pragma omp parallel for
	for(std::size_t n = 0; n < iteration_count; n++)
	{
		for(std::size_t i = 0; i < vec_c.size(); i++)
		{
			vec_c[i] = vec_a[i] + vec_b[i];
		}
	}

	et.stop();

	spdlog::info("Time to parallel compute vec_c on cpu: {} (nanoseconds)", et.count_nanoseconds());
	spdlog::info("Time to parallel compute vec_c on cpu: {} (milliseconds)", et.count_milliseconds());

	spdlog::info(
		"CPU check: {} {} {} {} {}",
		std::to_string(vec_c[0]),
		std::to_string(vec_c[1]),
		std::to_string(vec_c[1337]),
		std::to_string(vec_c[4096]),
		std::to_string(vec_c[vector_size - 1]));
    spdlog::info("CPU task completed.");
}