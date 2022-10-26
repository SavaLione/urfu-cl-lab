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
 * @brief Compute data on GPU
 * @author Saveliy Pototskiy (SavaLione)
 * @date 19 Oct 2022
 */
#ifndef COMPUTE_COMPUTE_GPU_H
#define COMPUTE_COMPUTE_GPU_H

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
#include <iterator>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

class compute_gpu
{
public:
	compute_gpu();
	~compute_gpu();

	void print_info();

	void compute_vec_16(std::string opencl_kernel_name);
	void compute_vec_8(std::string opencl_kernel_name);
	void compute_vec_4(std::string opencl_kernel_name);
	void compute_vec_2(std::string opencl_kernel_name);
	void compute_one_vec_16(std::string opencl_kernel_name);
	void compute_one_vec_8(std::string opencl_kernel_name);
	void compute_one_vec_4(std::string opencl_kernel_name);
	void compute_one_vec_2(std::string opencl_kernel_name);

private:
	/* Kernel loader instance */
	kernel_loader &kernel_loader_instance = kernel_loader::instance();

	std::vector<cl::Platform> all_platforms;
	cl::Platform default_platform;
	std::vector<cl::Device> all_devices;
	cl::Device default_device;
	cl::Context context;
	cl::Program::Sources sources;
	std::vector<std::string> kernels;
	cl::Program program;
	const std::size_t vector_size	  = 102400000;
	const std::size_t iteration_count = 100;

	void fill(std::vector<cl_float16> &vec_a, std::vector<cl_float16> &vec_b);
	void fill(std::vector<cl_float8> &vec_a, std::vector<cl_float8> &vec_b);
	void fill(std::vector<cl_float4> &vec_a, std::vector<cl_float4> &vec_b);
	void fill(std::vector<cl_float2> &vec_a, std::vector<cl_float2> &vec_b);
	void fill(std::vector<cl_float16> &vec_a);
	void fill(std::vector<cl_float8> &vec_a);
	void fill(std::vector<cl_float4> &vec_a);
	void fill(std::vector<cl_float2> &vec_a);

	void compact(std::vector<float> const &vec, std::vector<cl_float2> &v);
	void compact(std::vector<float> const &vec, std::vector<cl_float4> &v);
	void compact(std::vector<float> const &vec, std::vector<cl_float8> &v);
	void compact(std::vector<float> const &vec, std::vector<cl_float16> &v);
	void compact(std::vector<int> const &vec, std::vector<cl_int2> &v);
	void compact(std::vector<int> const &vec, std::vector<cl_int4> &v);
	void compact(std::vector<int> const &vec, std::vector<cl_int8> &v);
	void compact(std::vector<int> const &vec, std::vector<cl_int16> &v);

	template<typename iterator_type>
	void _compute(
		std::string opencl_application_name,
		iterator_type start_iterator_a,
		iterator_type end_iterator_a,
		iterator_type start_iterator_b,
		iterator_type end_iterator_b,
		iterator_type start_iterator_c,
		iterator_type end_iterator_c)
	{
		typedef typename std::iterator_traits<iterator_type>::value_type data_type;

		std::size_t size_a = sizeof(data_type) * (end_iterator_a - start_iterator_a);
		std::size_t size_b = sizeof(data_type) * (end_iterator_b - start_iterator_b);
		std::size_t size_c = sizeof(data_type) * (end_iterator_c - start_iterator_c);

		if((size_a != size_b) && (size_a != size_c))
		{
			throw std::logic_error("Iterators are not equal.");
		}

		spdlog::info("OpenCL application: {}", opencl_application_name);

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		cl::Buffer vec_buffer_a(context, start_iterator_a, end_iterator_a, true);
		cl::Buffer vec_buffer_b(context, start_iterator_b, end_iterator_b, true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, size_c);

		cl::Kernel kernel_simple_add(program, opencl_application_name.c_str());

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		//spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context, default_device);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		cl::NDRange global(end_iterator_a - start_iterator_a);
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, opencl_application_name);

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			kernel_funktor_simple_add(cl::EnqueueArgs(queue, global), vec_buffer_a, vec_buffer_b, vec_buffer_c).wait();
		}

		et.stop();

		cl::copy(queue, vec_buffer_c, start_iterator_c, end_iterator_c);

		spdlog::info("Time to parallel compute on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to parallel compute on gpu: {} (milliseconds)", et.count_milliseconds());
	}

	template<typename iterator_type>
	void _compute(
		std::string opencl_application_name,
		iterator_type start_iterator_a,
		iterator_type end_iterator_a,
		iterator_type start_iterator_c,
		iterator_type end_iterator_c)
	{
		typedef typename std::iterator_traits<iterator_type>::value_type data_type;

		std::size_t size_a = sizeof(data_type) * (end_iterator_a - start_iterator_a);
		std::size_t size_c = sizeof(data_type) * (end_iterator_c - start_iterator_c);

		if(size_a != size_c)
		{
			throw std::logic_error("Iterators are not equal.");
		}

		spdlog::info("OpenCL application: {}", opencl_application_name);

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		cl::Buffer vec_buffer_a(context, start_iterator_a, end_iterator_a, true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, size_c);

		cl::Kernel kernel_simple_add(program, opencl_application_name.c_str());

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		//spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context, default_device);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		cl::NDRange global(end_iterator_a - start_iterator_a);
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, opencl_application_name);

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			kernel_funktor_simple_add(cl::EnqueueArgs(queue, global), vec_buffer_a, vec_buffer_c).wait();
		}

		et.stop();

		cl::copy(queue, vec_buffer_c, start_iterator_c, end_iterator_c);

		spdlog::info("Time to parallel compute on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to parallel compute on gpu: {} (milliseconds)", et.count_milliseconds());
	}
};

/*
	OpenCL 1.2 data types:
		OpenCL Type				API Type	Description
		-----------				--------	-----------
		bool					--			true (1) or false (0)
		char 					cl_char 	8-bit signed
		unsigned char, uchar	cl_uchar 	8-bit unsigned
		short 					cl_short 	16-bit signed
		unsigned short, ushort 	cl_ushort 	16-bit unsigned
		int 					cl_int 		32-bit signed
		unsigned int, uint 		cl_uint 	32-bit unsigned
		long 					cl_long 	64-bit signed
		unsigned long, ulong 	cl_ulong 	64-bit unsigned
		float 					cl_float 	32-bit float
		double OPTIONAL 		cl_double	64-bit. IEEE 754
		half 					cl_half 	16-bit float (storage only)
		size_t 					-- 			32- or 64-bit unsigned integer
		ptrdiff_t 				-- 			32- or 64-bit signed integer
		intptr_t 				-- 			32- or 64-bit signed integer
		uintptr_t 				-- 			32- or 64-bit unsigned integer
		void 					void 		void
*/

#endif // COMPUTE_COMPUTE_GPU_H