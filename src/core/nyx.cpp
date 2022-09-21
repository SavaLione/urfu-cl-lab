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

#include <CL/cl.h>
#include <cstddef>

#define __CL_ENABLE_EXCEPTIONS

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.hpp>
#endif

#include "core/execution_time.h"

#include <chrono>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

void cl_platform();
void print_platform_info(cl::Platform const &platform, std::size_t id);

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
}

void cl_platform()
{
	std::vector<cl::Platform> cl_platforms;
	cl::Platform::get(&cl_platforms);

	spdlog::info("OpenCL platforms total: {}", cl_platforms.size());

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

	// cl::Platform default_platform = cl_platforms[0];
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

int main()
{
	cl_platform();
	return 0;
	const long long vector_size		= 102400000;
	const long long iteration_count = 100;

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
}
