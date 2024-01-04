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
 * @brief Compute
 * @author Savelii Pototskii (savalione.com)
 * @date 19 Oct 2022
 */
#include "compute/compute_gpu.h"

compute_gpu::compute_gpu(std::size_t const &vector_size, std::size_t const &iteration_count)
{
    this->vector_size     = vector_size;
    this->iteration_count = iteration_count;

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

void compute_gpu::compute_vec_16(std::string opencl_kernel_name)
{
    std::vector<cl_float16> vec_a_float_16(vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float16> vec_b_float_16(vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float16> vec_c_float_16(vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    /* Fill vectors */
    fill(vec_a_float_16, vec_b_float_16);

    _compute(opencl_kernel_name, vec_a_float_16.begin(), vec_a_float_16.end(), vec_b_float_16.begin(), vec_b_float_16.end(), vec_c_float_16.begin(), vec_c_float_16.end());
}

void compute_gpu::compute_vec_8(std::string opencl_kernel_name)
{
    std::vector<cl_float8> vec_a_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float8> vec_b_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float8> vec_c_float_8(vector_size / 8, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    /* Fill vectors */
    fill(vec_a_float_8, vec_b_float_8);

    _compute(opencl_kernel_name, vec_a_float_8.begin(), vec_a_float_8.end(), vec_b_float_8.begin(), vec_b_float_8.end(), vec_c_float_8.begin(), vec_c_float_8.end());
}

void compute_gpu::compute_vec_4(std::string opencl_kernel_name)
{
    std::vector<cl_float4> vec_a_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float4> vec_b_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float4> vec_c_float_4(vector_size / 4, {0.0, 0.0, 0.0, 0.0});

    /* Fill vectors */
    fill(vec_a_float_4, vec_b_float_4);

    _compute(opencl_kernel_name, vec_a_float_4.begin(), vec_a_float_4.end(), vec_b_float_4.begin(), vec_b_float_4.end(), vec_c_float_4.begin(), vec_c_float_4.end());
}

void compute_gpu::compute_vec_2(std::string opencl_kernel_name)
{
    std::vector<cl_float2> vec_a_float_2(vector_size / 2, {0.0, 0.0});
    std::vector<cl_float2> vec_b_float_2(vector_size / 2, {0.0, 0.0});
    std::vector<cl_float2> vec_c_float_2(vector_size / 2, {0.0, 0.0});

    /* Fill vectors */
    fill(vec_a_float_2, vec_b_float_2);

    _compute(opencl_kernel_name, vec_a_float_2.begin(), vec_a_float_2.end(), vec_b_float_2.begin(), vec_b_float_2.end(), vec_c_float_2.begin(), vec_c_float_2.end());
}

void compute_gpu::compute_one_vec_16(std::string opencl_kernel_name)
{
    std::vector<cl_float16> vec_a_float_16(vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<cl_float16> vec_c_float_16(vector_size / 16, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

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

void compute_gpu::run_all()
{
    // btw it works, but disabled 'cause it may cause troubles
    if(false)
        compute_lattice_2d("addition_lattice_2d");

    compute_vec_16("addition_vector_16");
    compute_vec_8("addition_vector_8");
    compute_vec_4("addition_vector_4");
    compute_vec_2("addition_vector_2");

    compute_vec_16("divide_vector_16");
    compute_vec_8("divide_vector_8");
    compute_vec_4("divide_vector_4");
    compute_vec_2("divide_vector_2");

    compute_vec_16("exponentiation_vector_16");
    compute_vec_8("exponentiation_vector_8");
    compute_vec_4("exponentiation_vector_4");
    compute_vec_2("exponentiation_vector_2");

    compute_vec_16("multiple_vector_16");
    compute_vec_8("multiple_vector_8");
    compute_vec_4("multiple_vector_4");
    compute_vec_2("multiple_vector_2");

    compute_vec_16("remove_vector_16");
    compute_vec_8("remove_vector_8");
    compute_vec_4("remove_vector_4");
    compute_vec_2("remove_vector_2");

    compute_one_vec_16("log_vector_16");
    compute_one_vec_16("log_vector_8");
    compute_one_vec_16("log_vector_4");
    compute_one_vec_16("log_vector_2");
}

void compute_gpu::compute_lattice_2d(std::string opencl_kernel_name)
{
    std::vector<cl_float2> vec_a_float(vector_size, {0.0, 0.0});
    std::vector<cl_float2> vec_b_float(vector_size, {0.0, 0.0});
    std::vector<cl_float2> vec_c_float(vector_size, {0.0, 0.0});

    /* Fill vectors */
    fill(vec_a_float, vec_b_float);

    _compute_lattice_2d(opencl_kernel_name, vec_a_float.begin(), vec_a_float.end(), vec_b_float.begin(), vec_b_float.end(), vec_c_float.begin(), vec_c_float.end());
}