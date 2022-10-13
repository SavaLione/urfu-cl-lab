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
 * @brief OpenCL
 * @author Saveliy Pototskiy (SavaLione)
 * @date 12 Oct 2022
 */
#include "compute/opencl.h"

#include <CL/cl_platform.h>

// clang-format off
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION  120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.h>
#endif
// clang-format on

#include "compute/kernel_loader.h"
#include "core/execution_time.h"

#include <CL/opencl.hpp>
#include <exception>
#include <spdlog/spdlog.h>

void compute_opencl_add()
{
	spdlog::info("GPU task started.");

	/* Kernel loader instance */
	kernel_loader &kernel_loader_instance = kernel_loader::instance();

	try
	{
		// Get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			spdlog::error("No OpenCL platforms found.");
			return;
		}
		cl::Platform default_platform = all_platforms[0];
		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		// Get default device of the default platform
		std::vector<cl::Device> all_devices;
		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if(all_devices.size() == 0)
		{
			spdlog::error("No OpenCL devices found.");
			return;
		}
		cl::Device default_device = all_devices[0];
		spdlog::info("Using OpenCL device: {}", default_device.getInfo<CL_DEVICE_NAME>());

		cl::Context context({default_device});

		cl::Program::Sources sources;

		std::vector<std::string> kernels = kernel_loader_instance.get();

		for(auto &kern : kernels)
		{
			sources.push_back({kern.c_str(), kern.length()});
		}

		cl::Program program(context, sources);
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
		catch(cl::Error const &err)
		{
			spdlog::error("OpenCL error.");
			spdlog::error(err.what());
			spdlog::error(err.err());
		}

		const std::size_t vector_size	  = 102400000;
		const std::size_t iteration_count = 100;

		std::vector<float> vec_a(vector_size, .0);
		std::vector<float> vec_b(vector_size, .0);
		std::vector<float> vec_c(vector_size, .0);

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

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		execution_time ett;
		ett.start();
		cl::Buffer vec_buffer_a(context, vec_a.begin(), vec_a.end(), true);
		cl::Buffer vec_buffer_b(context, vec_b.begin(), vec_b.end(), true);
		ett.stop();
		spdlog::info("Time to fill GPU buffers: {} nanoseconds", ett.count_nanoseconds());
		spdlog::info("Time to fill GPU buffers: {} milliseconds", ett.count_milliseconds());
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, sizeof(float) * vector_size);

		cl::Kernel kernel_simple_add(program, "simple_add");

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context, default_device);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		cl::NDRange global(vector_size);
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, "simple_add");

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			kernel_funktor_simple_add(cl::EnqueueArgs(queue, global), vec_buffer_a, vec_buffer_b, vec_buffer_c).wait();
		}

		et.stop();

		cl::copy(queue, vec_buffer_c, vec_c.begin(), vec_c.end());

		spdlog::info("Time to parallel compute vec_c on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to parallel compute vec_c on gpu: {} (milliseconds)", et.count_milliseconds());

		spdlog::info(
			"GPU check: {} {} {} {} {}",
			std::to_string(vec_c[0]),
			std::to_string(vec_c[1]),
			std::to_string(vec_c[1337]),
			std::to_string(vec_c[4096]),
			std::to_string(vec_c[vector_size - 1]));
	}
	catch(cl::Error &e)
	{
		spdlog::error("GPU error.");
		spdlog::error(e.what());
		spdlog::error(e.err());
	}

	spdlog::info("GPU task completed.");
}

void compute_opencl_vec_add()
{
	spdlog::info("GPU task started.");

	/* Kernel loader instance */
	kernel_loader &kernel_loader_instance = kernel_loader::instance();

	try
	{
		// Get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			spdlog::error("No OpenCL platforms found.");
			return;
		}
		cl::Platform default_platform = all_platforms[0];
		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		// Get default device of the default platform
		std::vector<cl::Device> all_devices;
		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if(all_devices.size() == 0)
		{
			spdlog::error("No OpenCL devices found.");
			return;
		}
		cl::Device default_device = all_devices[0];
		spdlog::info("Using OpenCL device: {}", default_device.getInfo<CL_DEVICE_NAME>());

		cl::Context context({default_device});

		cl::Program::Sources sources;

		std::vector<std::string> kernels = kernel_loader_instance.get();

		for(auto &kern : kernels)
		{
			sources.push_back({kern.c_str(), kern.length()});
		}

		cl::Program program(context, sources);
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
		catch(cl::Error const &err)
		{
			spdlog::error("OpenCL error.");
			spdlog::error(err.what());
			spdlog::error(err.err());
		}

		const std::size_t vector_size	  = 102400000;
		const std::size_t iteration_count = 100;

		std::vector<float> vec_a(vector_size, .0);
		std::vector<float> vec_b(vector_size, .0);
		std::vector<float> vec_c(vector_size, .0);

		std::vector<cl_float16> vec_a_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
		std::vector<cl_float16> vec_b_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
		std::vector<cl_float16> vec_c_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

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

		/* Fill vectors */
#pragma omp parallel for
		for(std::size_t i = 0; i < vec_a_float_16.size(); i++)
		{
			vec_a_float_16[i].s0 = vec_a[i * 16 + 0];
			vec_a_float_16[i].s1 = vec_a[i * 16 + 1];
			vec_a_float_16[i].s2 = vec_a[i * 16 + 2];
			vec_a_float_16[i].s3 = vec_a[i * 16 + 3];
			vec_a_float_16[i].s4 = vec_a[i * 16 + 4];
			vec_a_float_16[i].s5 = vec_a[i * 16 + 5];
			vec_a_float_16[i].s6 = vec_a[i * 16 + 6];
			vec_a_float_16[i].s7 = vec_a[i * 16 + 7];
			vec_a_float_16[i].s8 = vec_a[i * 16 + 8];
			vec_a_float_16[i].s9 = vec_a[i * 16 + 9];
			vec_a_float_16[i].sa = vec_a[i * 16 + 10];
			vec_a_float_16[i].sb = vec_a[i * 16 + 11];
			vec_a_float_16[i].sc = vec_a[i * 16 + 12];
			vec_a_float_16[i].sd = vec_a[i * 16 + 13];
			vec_a_float_16[i].se = vec_a[i * 16 + 14];
			vec_a_float_16[i].sf = vec_a[i * 16 + 15];
		}

#pragma omp parallel for
		for(std::size_t i = 0; i < vec_b_float_16.size(); i++)
		{
			vec_b_float_16[i].s0 = vec_b[i * 16 + 0];
			vec_b_float_16[i].s1 = vec_b[i * 16 + 1];
			vec_b_float_16[i].s2 = vec_b[i * 16 + 2];
			vec_b_float_16[i].s3 = vec_b[i * 16 + 3];
			vec_b_float_16[i].s4 = vec_b[i * 16 + 4];
			vec_b_float_16[i].s5 = vec_b[i * 16 + 5];
			vec_b_float_16[i].s6 = vec_b[i * 16 + 6];
			vec_b_float_16[i].s7 = vec_b[i * 16 + 7];
			vec_b_float_16[i].s8 = vec_b[i * 16 + 8];
			vec_b_float_16[i].s9 = vec_b[i * 16 + 9];
			vec_b_float_16[i].sa = vec_b[i * 16 + 10];
			vec_b_float_16[i].sb = vec_b[i * 16 + 11];
			vec_b_float_16[i].sc = vec_b[i * 16 + 12];
			vec_b_float_16[i].sd = vec_b[i * 16 + 13];
			vec_b_float_16[i].se = vec_b[i * 16 + 14];
			vec_b_float_16[i].sf = vec_b[i * 16 + 15];
		}

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		// cl::Buffer vec_buffer_a(context, vec_a.begin(), vec_a.end(), true);
		// cl::Buffer vec_buffer_b(context, vec_b.begin(), vec_b.end(), true);
		cl::Buffer vec_buffer_a(context, vec_a_float_16.begin(), vec_a_float_16.end(), true);
		cl::Buffer vec_buffer_b(context, vec_b_float_16.begin(), vec_b_float_16.end(), true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, sizeof(cl_float16) * (vector_size / 16));

		cl::Kernel kernel_simple_add(program, "simple_vec_add");

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context, default_device);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		cl::NDRange global(vector_size / 16);
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, "simple_vec_add");

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			kernel_funktor_simple_add(cl::EnqueueArgs(queue, global), vec_buffer_a, vec_buffer_b, vec_buffer_c).wait();
		}

		et.stop();

		cl::copy(queue, vec_buffer_c, vec_c_float_16.begin(), vec_c_float_16.end());

		/* Unpack vector */
#pragma omp parallel for
		for(std::size_t i = 0; i < vec_c_float_16.size(); i++)
		{
			vec_c[i * 16 + 0]  = vec_c_float_16[i].s0;
			vec_c[i * 16 + 1]  = vec_c_float_16[i].s1;
			vec_c[i * 16 + 2]  = vec_c_float_16[i].s2;
			vec_c[i * 16 + 3]  = vec_c_float_16[i].s3;
			vec_c[i * 16 + 4]  = vec_c_float_16[i].s4;
			vec_c[i * 16 + 5]  = vec_c_float_16[i].s5;
			vec_c[i * 16 + 6]  = vec_c_float_16[i].s6;
			vec_c[i * 16 + 7]  = vec_c_float_16[i].s7;
			vec_c[i * 16 + 8]  = vec_c_float_16[i].s8;
			vec_c[i * 16 + 9]  = vec_c_float_16[i].s9;
			vec_c[i * 16 + 10] = vec_c_float_16[i].sa;
			vec_c[i * 16 + 11] = vec_c_float_16[i].sb;
			vec_c[i * 16 + 12] = vec_c_float_16[i].sc;
			vec_c[i * 16 + 13] = vec_c_float_16[i].sd;
			vec_c[i * 16 + 14] = vec_c_float_16[i].se;
			vec_c[i * 16 + 15] = vec_c_float_16[i].sf;
		}

		spdlog::info("Time to parallel compute vec_c on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to parallel compute vec_c on gpu: {} (milliseconds)", et.count_milliseconds());

		spdlog::info(
			"GPU check: {} {} {} {} {}",
			std::to_string(vec_c[0]),
			std::to_string(vec_c[1]),
			std::to_string(vec_c[1337]),
			std::to_string(vec_c[4096]),
			std::to_string(vec_c[vector_size - 1]));
	}
	catch(cl::Error &e)
	{
		spdlog::error("GPU error.");
		spdlog::error(e.what());
		spdlog::error(e.err());
	}

	spdlog::info("GPU task completed.");
}

void compute_opencl_vector_addition_16()
{
	spdlog::info("GPU task started.");

	/* Kernel loader instance */
	kernel_loader &kernel_loader_instance = kernel_loader::instance();

	try
	{
		// Get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			spdlog::error("No OpenCL platforms found.");
			return;
		}
		cl::Platform default_platform = all_platforms[0];
		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		// Get default device of the default platform
		std::vector<cl::Device> all_devices;
		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if(all_devices.size() == 0)
		{
			spdlog::error("No OpenCL devices found.");
			return;
		}
		cl::Device default_device = all_devices[0];
		spdlog::info("Using OpenCL device: {}", default_device.getInfo<CL_DEVICE_NAME>());

		cl::Context context({default_device});

		cl::Program::Sources sources;

		std::vector<std::string> kernels = kernel_loader_instance.get();

		for(auto &kern : kernels)
		{
			sources.push_back({kern.c_str(), kern.length()});
		}

		cl::Program program(context, sources);
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
		catch(cl::Error const &err)
		{
			spdlog::error("OpenCL error.");
			spdlog::error(err.what());
			spdlog::error(err.err());
		}

		const std::size_t vector_size	  = 102400000;
		const std::size_t iteration_count = 100;

		std::vector<float> vec_c(vector_size, .0);

		std::vector<cl_float16> vec_a_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
		std::vector<cl_float16> vec_b_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
		std::vector<cl_float16> vec_c_float_16(
			vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});


		/* Fill vectors */
#pragma omp parallel for
		for(std::size_t i = 0; i < vec_a_float_16.size(); i++)
		{
			vec_a_float_16[i] = {
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
		for(std::size_t i = 0; i < vec_b_float_16.size(); i++)
		{
			vec_b_float_16[i] = {
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
				(float)(i + 15) * 3
			};
		}

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		cl::Buffer vec_buffer_a(context, vec_a_float_16.begin(), vec_a_float_16.end(), true);
		cl::Buffer vec_buffer_b(context, vec_b_float_16.begin(), vec_b_float_16.end(), true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, sizeof(cl_float16) * (vector_size / 16));

		cl::Kernel kernel_simple_add(program, "simple_vector_addition_16");

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context, default_device);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		cl::NDRange global(vector_size / 16);
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, "simple_vector_addition_16");

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			kernel_funktor_simple_add(cl::EnqueueArgs(queue, global), vec_buffer_a, vec_buffer_b, vec_buffer_c).wait();
		}

		et.stop();

		cl::copy(queue, vec_buffer_c, vec_c_float_16.begin(), vec_c_float_16.end());

		/* Unpack vector */
#pragma omp parallel for
		for(std::size_t i = 0; i < vec_c_float_16.size(); i++)
		{
			vec_c[i * 16 + 0]  = vec_c_float_16[i].s0;
			vec_c[i * 16 + 1]  = vec_c_float_16[i].s1;
			vec_c[i * 16 + 2]  = vec_c_float_16[i].s2;
			vec_c[i * 16 + 3]  = vec_c_float_16[i].s3;
			vec_c[i * 16 + 4]  = vec_c_float_16[i].s4;
			vec_c[i * 16 + 5]  = vec_c_float_16[i].s5;
			vec_c[i * 16 + 6]  = vec_c_float_16[i].s6;
			vec_c[i * 16 + 7]  = vec_c_float_16[i].s7;
			vec_c[i * 16 + 8]  = vec_c_float_16[i].s8;
			vec_c[i * 16 + 9]  = vec_c_float_16[i].s9;
			vec_c[i * 16 + 10] = vec_c_float_16[i].sa;
			vec_c[i * 16 + 11] = vec_c_float_16[i].sb;
			vec_c[i * 16 + 12] = vec_c_float_16[i].sc;
			vec_c[i * 16 + 13] = vec_c_float_16[i].sd;
			vec_c[i * 16 + 14] = vec_c_float_16[i].se;
			vec_c[i * 16 + 15] = vec_c_float_16[i].sf;
		}

		spdlog::info("Time to parallel compute vec_c on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to parallel compute vec_c on gpu: {} (milliseconds)", et.count_milliseconds());

		spdlog::info(
			"GPU check: {} {} {} {} {}",
			std::to_string(vec_c[0]),
			std::to_string(vec_c[1]),
			std::to_string(vec_c[1337]),
			std::to_string(vec_c[4096]),
			std::to_string(vec_c[vector_size - 1]));
	}
	catch(cl::Error &e)
	{
		spdlog::error("GPU error.");
		spdlog::error(e.what());
		spdlog::error(e.err());
	}

	spdlog::info("GPU task completed.");
}