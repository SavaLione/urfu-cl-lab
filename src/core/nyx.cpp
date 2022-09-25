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
 * @brief Nyx application
 * @author Saveliy Pototskiy (SavaLione)
 * @date 21 Sep 2022
 */
#include "core/nyx.h"

#define CL_HPP_ENABLE_EXCEPTIONS
// #define CL_HPP_TARGET_OPENCL_VERSION 120
// #define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION  120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.h>
#endif

#include "core/execution_time.h"

#include <CL/opencl.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

void cl_platform();
void print_platform_info(cl::Platform const &platform, std::size_t id);
void cltask();
void cl_task_new();

static cl_int cl_allocate_and_get_info(cl_device_id const &device, cl_device_info const &param_name, std::string &param_value)
{
	cl_int error_code = CL_SUCCESS;

	if(param_name == NULL)
	{
		spdlog::warn("cl_allocate_and_get_info param_name == NULL");
		return error_code;
	}

	if(error_code == CL_SUCCESS)
	{
		error_code = clGetDeviceInfo(device, param_name, 0, NULL, NULL);

		if(error_code != CL_SUCCESS)
		{
			spdlog::error("OpenCL can't get device info.");
			return error_code;
		}
	}

	// if(error_code == CL_SUCCESS)
	// {
	// 	error_code = clGetDeviceInfo(device, param_name, size, param_value, NULL);
	// }
	return CL_SUCCESS;
}

void cl_platform()
{
	std::vector<cl::Platform> cl_platforms;
	cl::Platform::get(&cl_platforms);

	cl::Platform cl_platform_default;

	spdlog::info("OpenCL platforms total: {}", cl_platforms.size());

	for(auto &p : cl_platforms)
	{
		std::string platform_name	 = p.getInfo<CL_PLATFORM_NAME>();
		std::string platform_version = p.getInfo<CL_PLATFORM_VERSION>();
		spdlog::info("OpenCL platform: {} version: {}", platform_name, platform_version);

		cl_platform_default = p;
	}

	if(cl_platforms.size() == 0)
	{
		spdlog::error("No OpenCL platforms found.");
		return;
	}

	for(std::size_t i = 0; i < cl_platforms.size(); i++)
	{
		print_platform_info(cl_platforms[i], i);

		std::vector<cl::Device> cl_devices;
		cl_platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &cl_devices);

		if(cl_devices.size() == 0)
		{
			spdlog::error("No OpenCL platform id {} devices found.", i);
		}

		spdlog::info("\n\t\tNumber of devices: {}.", cl_devices.size());

		for(std::size_t d = 0; d < cl_devices.size(); d++)
		{
			std::string info = "";

			info += "\n\t\t\tDevice id: ";
			info += std::to_string(d);

			info += "\n\t\t\t\tName: ";
			info += cl_devices[d].getInfo<CL_DEVICE_NAME>();

			info += "\n\t\t\t\tType: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_TYPE>());
			info += " ";
			switch(cl_devices[d].getInfo<CL_DEVICE_TYPE>())
			{
				case CL_DEVICE_TYPE_ACCELERATOR:
					info += "ACCELERATOR";
					break;
				case CL_DEVICE_TYPE_CPU:
					info += "CPU";
					break;
				case CL_DEVICE_TYPE_CUSTOM:
					info += "CUSTOM";
					break;
				case CL_DEVICE_TYPE_DEFAULT:
					info += "DEFAULT";
					break;
				case CL_DEVICE_TYPE_ALL:
					info += "ALL";
					break;
				default:
					info += "undefined";
					break;
			}

			info += "\n\t\t\t\tVendor: ";
			info += cl_devices[d].getInfo<CL_DEVICE_VENDOR>();

			info += "\n\t\t\t\tMax Compute Units: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());

			info += "\n\t\t\t\tGlobal Memory: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>());

			info += "\n\t\t\t\tMax Clock Frequency:: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>());

			info += "\n\t\t\t\tMax Allocateable Memory: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>());

			info += "\n\t\t\t\tLocal Memory: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>());

			info += "\n\t\t\t\tAvailable: ";
			info += std::to_string(cl_devices[d].getInfo<CL_DEVICE_AVAILABLE>());

			spdlog::info(info);
		}
	}

	//

	cl::Platform cl_platform_new = cl::Platform::setDefault(cl_platform_default);
	if(cl_platform_new != cl_platform_default)
	{
		spdlog::error("Error setting default platform.");
		return;
	}

	// C++11 raw string literal for the first kernel
	// std::string kernel1 {R"CLC(
	//     global int globalA;
	//     kernel void updateGlobal()
	//     {
	//       globalA = 75;
	//     }
	// )CLC"};

	std::string kernel1 {R"CLC(
		__kernel void sup()
		{
		}
    )CLC"};

	// Raw string literal for the second kernel
	// std::string kernel2 {R"CLC(
	//     typedef struct { global int *bar; } Foo;
	//     kernel void vectorAdd(global const Foo* aNum, global const int *inputA, global const int *inputB,
	//                           global int *output, int val, write_only pipe int outPipe, queue_t childQueue)
	//     {
	//       output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)] + val + *(aNum->bar);
	//       write_pipe(outPipe, &val);
	//       queue_t default_queue = get_default_queue();
	//       ndrange_t ndrange = ndrange_1D(get_global_size(0)/2, get_global_size(0)/2);

	//       // Have a child kernel write into third quarter of output
	//       enqueue_kernel(default_queue, CLK_ENQUEUE_FLAGS_WAIT_KERNEL, ndrange,
	//         ^{
	//             output[get_global_size(0)*2 + get_global_id(0)] =
	//               inputA[get_global_size(0)*2 + get_global_id(0)] + inputB[get_global_size(0)*2 + get_global_id(0)] + globalA;
	//         });

	//       // Have a child kernel write into last quarter of output
	//       enqueue_kernel(childQueue, CLK_ENQUEUE_FLAGS_WAIT_KERNEL, ndrange,
	//         ^{
	//             output[get_global_size(0)*3 + get_global_id(0)] =
	//               inputA[get_global_size(0)*3 + get_global_id(0)] + inputB[get_global_size(0)*3 + get_global_id(0)] + globalA + 2;
	//         });
	//     }
	// )CLC"};

	std::string kernel2 {R"CLC(
		__kernel void simple_add(__global const int* A, __global const int* B, __global int* C)
		{
			int index = get_global_id(0);
			C[index] = A[index] + B[index];
		}
    )CLC"};

	std::vector<std::string> cl_program_strings;
	cl_program_strings.push_back(kernel1);
	cl_program_strings.push_back(kernel2);

	cl::Program cl_vector_add_program(cl_program_strings);

	try
	{
		// cl_vector_add_program.build("-cl-std=CL2.0");
		cl_vector_add_program.build("-cl-std=CL1.2");
	}
	catch(...)
	{
		// Print build info for all devices
		cl_int build_err = CL_SUCCESS;
		auto build_info	 = cl_vector_add_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&build_err);
		for(auto &pair : build_info)
		{
			spdlog::error("Error building kernel.");
			spdlog::error(pair.second);
		}

		return;
	}

	// Get and run kernel that initializes the program-scope global
	// A test for kernels that take no arguments
	// auto program_2_kernel = cl::KernelFunctor<>(cl_vector_add_program, "updateGlobal");
	// program_2_kernel(cl::EnqueueArgs(cl::NDRange(1)));

	///
	std::vector<cl::Device> all_devices;
	cl_platform_default.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

	if(all_devices.size() == 0)
	{
		spdlog::error("No OpenCL devices found.");
		return;
	}

	cl::Device cl_device_default = all_devices[0];
	spdlog::info("Using OpenCL device: {}", cl_device_default.getInfo<CL_DEVICE_NAME>());

	cl::Context context({cl_device_default});

	cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

	int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, cl_device_default);

	//write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);

	cl::Kernel kernel(cl_vector_add_program, "simple_add");

	kernel.setArg(0, buffer_A);
	kernel.setArg(1, buffer_B);
	kernel.setArg(2, buffer_C);

	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(10), cl::NullRange);

	int C[10];

	//read result C from the device to array C
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);
	queue.finish();

	std::string arr_a = "";
	std::string arr_b = "";

	for(int i = 0; i < 10; i++)
	{
		arr_a += std::to_string(A[i]);
		arr_a += " ";
		arr_b += std::to_string(B[i]);
		arr_b += " ";
	}

	spdlog::info("A:      {}", arr_a);
	spdlog::info("B:      {}", arr_b);

	std::string result = "";
	for(int i = 0; i < 10; i++)
	{
		result += std::to_string(C[i]);
		result += " ";
	}
	spdlog::info("Result: {}", result);
}

void print_platform_info(cl::Platform const &platform, std::size_t id)
{
	std::string platform_name;
	std::string platform_vendor;
	std::string platform_version;

	cl_int error;

	error = platform.getInfo(CL_PLATFORM_NAME, &platform_name);

	if(error != CL_SUCCESS)
	{
		spdlog::error("print_platform_info(): Error during query for CL_PLATFORM_NAME: ", std::to_string(error));
	}

	error = platform.getInfo(CL_PLATFORM_VENDOR, &platform_vendor);

	if(error != CL_SUCCESS)
	{
		spdlog::error("print_platform_info(): Error during query for CL_PLATFORM_VENDOR: ", std::to_string(error));
	}

	error = platform.getInfo(CL_PLATFORM_VERSION, &platform_version);

	if(error != CL_SUCCESS)
	{
		spdlog::error("print_platform_info(): Error during query for CL_PLATFORM_VERSION: ", std::to_string(error));
	}

	spdlog::info(
		"OpenCL platform info:\n\tid: {}\n\tName: {}\n\tVendor: {}\n\tVersion: {}",
		id,
		platform_name,
		platform_vendor,
		platform_version);
}

void cl_task_new()
{
	spdlog::info("New GPU task started.");

	try
	{
		//get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			spdlog::error("No OpenCL platforms found.");
			return;
		}
		cl::Platform default_platform = all_platforms[0];
		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		//get default device of the default platform
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

		// kernel calculates for each element C=A+B
		std::string kernel_code =
			"__kernel void simple_add(__global const float* A, __global const float* B, __global float* C) {"
			"	int index = get_global_id(0);"
			"	C[index] = A[index] + B[index];"
			"};";
		sources.push_back({kernel_code.c_str(), kernel_code.length()});

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

		// const std::size_t vector_size = 102400000 / 8192;
		const std::size_t vector_size = 102400000;

		std::vector<float> vec_a(vector_size), vec_b(vector_size), vec_c(vector_size);

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
		cl::Buffer vec_buffer_a(context, vec_a.begin(), vec_a.end(), true);
		cl::Buffer vec_buffer_b(context, vec_b.begin(), vec_b.end(), true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, sizeof(float) * vector_size);

		cl::Kernel kernel_simple_add(program, "simple_add");

		std::size_t local_work_group_size =
			kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());
		spdlog::info("OpenGL kernel work group size: {}", local_work_group_size);

		cl::CommandQueue queue(context);

		/*
			We need to specify global (and local) dimensions
			cl::NDRange global(1024);
			cl::NDRange local(64)

			If you don’t specify a local dimension, it is assumed as cl::NullRange, and
			the runtime picks a size for you
		*/
		// kernel_simple_add(cl::EnqueueArgs(queue, cl::NDRange(vector_size)), vec_buffer_a, vec_buffer_b, vec_buffer_c);
		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_funktor_simple_add(program, "simple_add");

		kernel_funktor_simple_add(cl::EnqueueArgs(queue, cl::NDRange(256)), vec_buffer_a, vec_buffer_b, vec_buffer_c);

		cl::copy(queue, vec_buffer_c, vec_c.begin(), vec_c.end());

		spdlog::info("GPU check: {} {} {}", std::to_string(vec_c[0]), std::to_string(vec_c[1]), std::to_string(vec_c[128]));
	}
	catch(cl::Error &e)
	{
		spdlog::error("GPU error.");
		spdlog::error(e.what());
		spdlog::error(e.err());
	}

	spdlog::info("New GPU task completed.");
}

void cltask()
{
	spdlog::info("GPU task started.");
	try
	{
		//get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			spdlog::error("No OpenCL platforms found.");
			return;
		}
		cl::Platform default_platform = all_platforms[0];
		spdlog::info("Using OpenCL platform: {}", default_platform.getInfo<CL_PLATFORM_NAME>());

		//get default device of the default platform
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

		// kernel calculates for each element C=A+B
		std::string kernel_code =
			"__kernel void simple_add(__global const float* A, __global const float* B, __global float* C) {"
			"	int index = get_global_id(0);"
			"	C[index] = A[index] + B[index];"
			"};";
		sources.push_back({kernel_code.c_str(), kernel_code.length()});

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

		///

		// const std::size_t vector_size	  = 102400000;
		const std::size_t iteration_count = 1;
		const std::size_t vector_size	  = 102400000 / 8192;
		// const long long iteration_count = 100;

		float arr_a[vector_size];
		float arr_b[vector_size];

		std::vector<float> vec_a(vector_size), vec_b(vector_size), vec_c(vector_size);

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

		// 		std::vector<float> vec_a(vector_size, 0);
		// 		std::vector<float> vec_b(vector_size, 0);
		// 		std::vector<float> vec_c(vector_size, 0);

		// 		spdlog::info("vec_a size: {}", vec_a.size());
		// 		spdlog::info("vec_b size: {}", vec_b.size());
		// 		spdlog::info("vec_c size: {}", vec_c.size());

		// #pragma omp parallel for
		// 		for(std::size_t i = 0; i < vec_a.size(); i++)
		// 		{
		// 			vec_a[i] = (float)i / 2;
		// 		}

		// #pragma omp parallel for
		// 		for(std::size_t i = 0; i < vec_b.size(); i++)
		// 		{
		// 			vec_b[i] = (float)i * 3;
		// 		}

#pragma omp parallel for
		for(std::size_t i = 0; i < vector_size; i++)
		{
			arr_a[i] = (float)i / 2;
		}

#pragma omp parallel for
		for(std::size_t i = 0; i < vector_size; i++)
		{
			arr_b[i] = (float)i * 3;
		}

		/* Create buffers on the device */
		// cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(float) * vector_size);
		// cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(float) * vector_size);
		// cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(float) * vector_size);

		cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, sizeof(float) * vector_size);
		cl::Buffer buffer_B(context, CL_MEM_READ_ONLY, sizeof(float) * vector_size);
		cl::Buffer buffer_C(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(float) * vector_size);

		/*
			true means that this is a read-only buffer
			(false) means: read/write (default)
		*/
		cl::Buffer vec_buffer_a(context, vec_a.begin(), vec_a.end(), true);
		cl::Buffer vec_buffer_b(context, vec_b.begin(), vec_b.end(), true);
		cl::Buffer vec_buffer_c(context, CL_MEM_WRITE_ONLY, sizeof(float) * vector_size);

		cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel_simple_add(program, "simple_add");

		//create queue to which we will push commands for the device.
		cl::CommandQueue queue(context, default_device);

		//write arrays A and B to the device
		// queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
		// queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);
		// queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(float) * vector_size, static_cast<void *>(vec_a.data()));
		// queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(float) * vector_size, static_cast<void *>(vec_b.data()));
		queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(float) * vector_size, arr_a);
		queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(float) * vector_size, arr_b);

		execution_time et;
		et.start();

		for(std::size_t n = 0; n < iteration_count; n++)
		{
			cl::Kernel kernel(program, "simple_add");

			kernel.setArg(0, buffer_A);
			kernel.setArg(1, buffer_B);
			kernel.setArg(2, buffer_C);

			queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(vector_size), cl::NullRange);

			float C[vector_size];
			//read result C from the device to array C
			queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(float) * vector_size, C);
			queue.finish();

			// spdlog::info("GPU check: {} {} {}", C[0], C[1], C[2839]);
			// spdlog::info("GPU check: {} {} {}", std::to_string(C[0]), std::to_string(C[1]), std::to_string(C[128]));
		}
		et.stop();

		spdlog::info("Time to compute data on gpu: {} (nanoseconds)", et.count_nanoseconds());
		spdlog::info("Time to compute data on gpu: {} (milliseconds)", et.count_milliseconds());
	}
	catch(cl::Error &e)
	{
		spdlog::error("GPU error.");
		spdlog::error(e.what());
		spdlog::error(e.err());
	}
	spdlog::info("GPU task completed.");
}

int main()
{
	// cltask();
	cl_task_new();
	return 0;

	// 	const long long vector_size		= 102400000;
	// 	const long long iteration_count = 100;

	// 	std::vector<float> vec_a(vector_size, 0);
	// 	std::vector<float> vec_b(vector_size, 0);
	// 	std::vector<float> vec_c(vector_size, 0);

	// 	spdlog::info("vec_a size: {}", vec_a.size());
	// 	spdlog::info("vec_b size: {}", vec_b.size());
	// 	spdlog::info("vec_c size: {}", vec_c.size());

	// 	execution_time et;
	// 	et.start();

	// #pragma omp parallel for
	// 	for(std::size_t i = 0; i < vec_a.size(); i++)
	// 	{
	// 		vec_a[i] = (float)i / 2;
	// 	}

	// #pragma omp parallel for
	// 	for(std::size_t i = 0; i < vec_b.size(); i++)
	// 	{
	// 		vec_b[i] = (float)i * 3;
	// 	}

	// 	et.stop();

	// 	spdlog::info("Time to parallel fill vectors: {} (nanoseconds)", et.count_nanoseconds());
	// 	spdlog::info("Time to parallel fill vectors: {} (milliseconds)", et.count_milliseconds());

	// 	et.start();

	// 	for(std::size_t n = 0; n < iteration_count; n++)
	// 	{
	// 		for(std::size_t i = 0; i < vec_c.size(); i++)
	// 		{
	// 			vec_c[i] = vec_a[i] + vec_b[i];
	// 		}
	// 	}

	// 	et.stop();

	// 	spdlog::info("Time to compute vec_c on cpu: {} (nanoseconds)", et.count_nanoseconds());
	// 	spdlog::info("Time to compute vec_c on cpu: {} (milliseconds)", et.count_milliseconds());

	// 	et.start();

	// #pragma omp parallel for
	// 	for(std::size_t n = 0; n < iteration_count; n++)
	// 	{
	// 		for(std::size_t i = 0; i < vec_c.size(); i++)
	// 		{
	// 			vec_c[i] = vec_a[i] + vec_b[i];
	// 		}
	// 	}

	// 	et.stop();

	// 	spdlog::info("Time to parallel compute vec_c on cpu: {} (nanoseconds)", et.count_nanoseconds());
	// 	spdlog::info("Time to parallel compute vec_c on cpu: {} (milliseconds)", et.count_milliseconds());

	// 	spdlog::info("CPU check: {} {} {}", vec_c[0], vec_c[1], vec_c[2839]);
}
