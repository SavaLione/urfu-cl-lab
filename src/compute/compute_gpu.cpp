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
 * @brief Compute
 * @author Saveliy Pototskiy (SavaLione)
 * @date 19 Oct 2022
 */
#include "compute/compute_gpu.h"

#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <string>

compute_gpu::compute_gpu()
{
	try
	{
		cl::Platform::get(&all_platforms);

		if(all_platforms.size() == 0)
		{
			throw std::runtime_error("No OpenCL platforms found.");
		}

		default_platform = all_platforms[0];

		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

		if(all_devices.size() == 0)
		{
			throw std::runtime_error("No OpenCL devices found.");
		}

		default_device = all_devices[0];

		spdlog::info("Using OpenCL device: {}", default_device.getInfo<CL_DEVICE_NAME>());

		context = cl::Context({default_device});

		kernels = kernel_loader_instance.get();

		for(auto &kern : kernels)
		{
			sources.push_back({kern.c_str(), kern.length()});
		}

		program = cl::Program(context, sources);

		try
		{
			program.build({default_device});
		}
		catch(cl::BuildError err)
		{
			spdlog::error("OpenCL build error.");
			spdlog::error("Error OpenCL building: {}", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device));
			return;
		}
	}
	catch(cl::Error &e)
	{
		spdlog::error("OpenCL error: {}", e.what());
		spdlog::error(e.err());
	}
	catch(std::exception const &e)
	{
		spdlog::error(e.what());
	}
	catch(...)
	{
		spdlog::error("Unexpected exception.");
	}
}

compute_gpu::~compute_gpu() {}

void compute_gpu::print_info()
{
	try
	{
		spdlog::info("Platform name: {}", default_platform.getInfo<CL_PLATFORM_NAME>());
		spdlog::info("Platform vendor: {}", default_platform.getInfo<CL_PLATFORM_VENDOR>());
		spdlog::info("Platform extensions: {}", default_platform.getInfo<CL_PLATFORM_EXTENSIONS>());
		spdlog::info("Platform profile: {}", default_platform.getInfo<CL_PLATFORM_PROFILE>());
		spdlog::info("Platform version: {}", default_platform.getInfo<CL_PLATFORM_VERSION>());

		spdlog::info("Device name: {}", default_device.getInfo<CL_DEVICE_NAME>());
		spdlog::info("Device vendor: {}", default_device.getInfo<CL_DEVICE_VENDOR>());
		spdlog::info("Device version: {}", default_device.getInfo<CL_DEVICE_VERSION>());
		spdlog::info("Device CL_DEVICE_BUILT_IN_KERNELS: {}", default_device.getInfo<CL_DEVICE_BUILT_IN_KERNELS>());
		spdlog::info("Device extensions: {}", default_device.getInfo<CL_DEVICE_EXTENSIONS>());
		//spdlog::info("Device CL_DEVICE_MAX_WORK_ITEM_SIZES: {}", default_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>());
		spdlog::info("Device CL_DEVICE_OPENCL_C_VERSION: {}", default_device.getInfo<CL_DEVICE_OPENCL_C_VERSION>());
		//spdlog::info("Device CL_DEVICE_PARTITION_PROPERTIES: {}", default_device.getInfo<CL_DEVICE_PARTITION_PROPERTIES>());
		//spdlog::info("Device CL_DEVICE_PARTITION_TYPE: {}", default_device.getInfo<CL_DEVICE_PARTITION_TYPE>());
		spdlog::info("Device CL_DEVICE_PROFILE: {}", default_device.getInfo<CL_DEVICE_PROFILE>());
		spdlog::info("Device CL_DEVICE_MAX_CLOCK_FREQUENCY: {}", default_device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>());
		spdlog::info("Device CL_DEVICE_MAX_MEM_ALLOC_SIZE: {}", default_device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>());
		spdlog::info("Device CL_DEVICE_GLOBAL_MEM_SIZE: {}", default_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>());
		spdlog::info("Device CL_DEVICE_LOCAL_MEM_SIZE: {}", default_device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>());
		spdlog::info("Device CL_DEVICE_AVAILABLE: {}", default_device.getInfo<CL_DEVICE_AVAILABLE>());
	}
	catch(cl::Error &e)
	{
		spdlog::error("OpenCL error: {}", e.what());
		spdlog::error(e.err());
	}
}

void compute_gpu::fill(std::vector<cl_float16> &vec_a, std::vector<cl_float16> &vec_b)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {
			(float)(i + 0) / 2,
			(float)(i + 1) / 2,
			(float)(i + 2) / 2,
			(float)(i + 3) / 2,
			(float)(i + 4) / 2,
			(float)(i + 5) / 2,
			(float)(i + 6) / 2,
			(float)(i + 7) / 2,
			(float)(i + 8) / 2,
			(float)(i + 9) / 2,
			(float)(i + 10) / 2,
			(float)(i + 11) / 2,
			(float)(i + 12) / 2,
			(float)(i + 13) / 2,
			(float)(i + 14) / 2,
			(float)(i + 15) / 2};
	}

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_b.size(); i++)
	{
		vec_b[i] = {
			(float)(i + 0) * 3,
			(float)(i + 1) * 3,
			(float)(i + 2) * 3,
			(float)(i + 3) * 3,
			(float)(i + 4) * 3,
			(float)(i + 5) * 3,
			(float)(i + 6) * 3,
			(float)(i + 7) * 3,
			(float)(i + 8) * 3,
			(float)(i + 9) * 3,
			(float)(i + 10) * 3,
			(float)(i + 11) * 3,
			(float)(i + 12) * 3,
			(float)(i + 13) * 3,
			(float)(i + 14) * 3,
			(float)(i + 15) * 3};
	}
}

void compute_gpu::fill(std::vector<cl_float8> &vec_a, std::vector<cl_float8> &vec_b)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {
			(float)(i + 0) / 2,
			(float)(i + 1) / 2,
			(float)(i + 2) / 2,
			(float)(i + 3) / 2,
			(float)(i + 4) / 2,
			(float)(i + 5) / 2,
			(float)(i + 6) / 2,
			(float)(i + 7) / 2};
	}

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_b.size(); i++)
	{
		vec_b[i] = {
			(float)(i + 0) * 3,
			(float)(i + 1) * 3,
			(float)(i + 2) * 3,
			(float)(i + 3) * 3,
			(float)(i + 4) * 3,
			(float)(i + 5) * 3,
			(float)(i + 6) * 3,
			(float)(i + 7) * 3};
	}
}

void compute_gpu::fill(std::vector<cl_float4> &vec_a, std::vector<cl_float4> &vec_b)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {(float)(i + 0) / 2, (float)(i + 1) / 2, (float)(i + 2) / 2, (float)(i + 3) / 2};
	}

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_b.size(); i++)
	{
		vec_b[i] = {(float)(i + 0) * 3, (float)(i + 1) * 3, (float)(i + 2) * 3, (float)(i + 3) * 3};
	}
}

void compute_gpu::fill(std::vector<cl_float2> &vec_a, std::vector<cl_float2> &vec_b)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {(float)(i + 0) / 2, (float)(i + 1) / 2};
	}

#pragma omp parallel for
	for(std::size_t i = 0; i < vec_b.size(); i++)
	{
		vec_b[i] = {(float)(i + 0) * 3, (float)(i + 1) * 3};
	}
}

void compute_gpu::fill(std::vector<cl_float16> &vec_a)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {
			(float)(i + 0) / 2,
			(float)(i + 1) / 2,
			(float)(i + 2) / 2,
			(float)(i + 3) / 2,
			(float)(i + 4) / 2,
			(float)(i + 5) / 2,
			(float)(i + 6) / 2,
			(float)(i + 7) / 2,
			(float)(i + 8) / 2,
			(float)(i + 9) / 2,
			(float)(i + 10) / 2,
			(float)(i + 11) / 2,
			(float)(i + 12) / 2,
			(float)(i + 13) / 2,
			(float)(i + 14) / 2,
			(float)(i + 15) / 2};
	}
}

void compute_gpu::fill(std::vector<cl_float8> &vec_a)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {
			(float)(i + 0) / 2,
			(float)(i + 1) / 2,
			(float)(i + 2) / 2,
			(float)(i + 3) / 2,
			(float)(i + 4) / 2,
			(float)(i + 5) / 2,
			(float)(i + 6) / 2,
			(float)(i + 7) / 2};
	}
}

void compute_gpu::fill(std::vector<cl_float4> &vec_a)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {(float)(i + 0) / 2, (float)(i + 1) / 2, (float)(i + 2) / 2, (float)(i + 3) / 2};
	}
}

void compute_gpu::fill(std::vector<cl_float2> &vec_a)
{
#pragma omp parallel for
	for(std::size_t i = 0; i < vec_a.size(); i++)
	{
		vec_a[i] = {(float)(i + 0) / 2, (float)(i + 1) / 2};
	}
}

void compute_gpu::compute_vec_16(std::string opencl_kernel_name)
{
	std::vector<cl_float16> vec_a_float_16(
		vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float16> vec_b_float_16(
		vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float16> vec_c_float_16(
		vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_16, vec_b_float_16);

	_compute(
		opencl_kernel_name,
		vec_a_float_16.begin(),
		vec_a_float_16.end(),
		vec_b_float_16.begin(),
		vec_b_float_16.end(),
		vec_c_float_16.begin(),
		vec_c_float_16.end());
}

void compute_gpu::compute_vec_8(std::string opencl_kernel_name)
{
	std::vector<cl_float8> vec_a_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float8> vec_b_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float8> vec_c_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_8, vec_b_float_8);

	_compute(
		opencl_kernel_name,
		vec_a_float_8.begin(),
		vec_a_float_8.end(),
		vec_b_float_8.begin(),
		vec_b_float_8.end(),
		vec_c_float_8.begin(),
		vec_c_float_8.end());
}

void compute_gpu::compute_vec_4(std::string opencl_kernel_name)
{
	std::vector<cl_float4> vec_a_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float4> vec_b_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float4> vec_c_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_4, vec_b_float_4);

	_compute(
		opencl_kernel_name,
		vec_a_float_4.begin(),
		vec_a_float_4.end(),
		vec_b_float_4.begin(),
		vec_b_float_4.end(),
		vec_c_float_4.begin(),
		vec_c_float_4.end());
}

void compute_gpu::compute_vec_2(std::string opencl_kernel_name)
{
	std::vector<cl_float2> vec_a_float_2(vector_size / 2, {0.0, 0.0});
	std::vector<cl_float2> vec_b_float_2(vector_size / 2, {0.0, 0.0});
	std::vector<cl_float2> vec_c_float_2(vector_size / 2, {0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_2, vec_b_float_2);

	_compute(
		opencl_kernel_name,
		vec_a_float_2.begin(),
		vec_a_float_2.end(),
		vec_b_float_2.begin(),
		vec_b_float_2.end(),
		vec_c_float_2.begin(),
		vec_c_float_2.end());
}

void compute_gpu::compute_one_vec_16(std::string opencl_kernel_name)
{
	std::vector<cl_float16> vec_a_float_16(
		vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float16> vec_c_float_16(
		vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_16);

	_compute(opencl_kernel_name, vec_a_float_16.begin(), vec_a_float_16.end(), vec_c_float_16.begin(), vec_c_float_16.end());
}

void compute_gpu::compute_one_vec_8(std::string opencl_kernel_name)
{
	std::vector<cl_float8> vec_a_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float8> vec_c_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_8);

	_compute(opencl_kernel_name, vec_a_float_8.begin(), vec_a_float_8.end(), vec_c_float_8.begin(), vec_c_float_8.end());
}

void compute_gpu::compute_one_vec_4(std::string opencl_kernel_name)
{
	std::vector<cl_float4> vec_a_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});
	std::vector<cl_float4> vec_c_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_4);

	_compute(opencl_kernel_name, vec_a_float_4.begin(), vec_a_float_4.end(), vec_c_float_4.begin(), vec_c_float_4.end());
}

void compute_gpu::compute_one_vec_2(std::string opencl_kernel_name)
{
	std::vector<cl_float2> vec_a_float_2(vector_size / 2, {0.0, 0.0});
	std::vector<cl_float2> vec_c_float_2(vector_size / 2, {0.0, 0.0});

	/* Fill vectors */
	fill(vec_a_float_2);

	_compute(opencl_kernel_name, vec_a_float_2.begin(), vec_a_float_2.end(), vec_c_float_2.begin(), vec_c_float_2.end());
}