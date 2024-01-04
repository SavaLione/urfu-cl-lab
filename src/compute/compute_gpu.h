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
 * @brief Compute data on GPU
 * @author Savelii Pototskii (savalione.com)
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

#include "core/execution_time.h"
#include "io/log/logger.h"
#include "io/kernel_loader.h"

#include <CL/opencl.hpp>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>

class compute_gpu
{
public:
    compute_gpu(std::size_t const &vector_size, std::size_t const &iteration_count);

    void print_info();

    void run_all();

    void compute_vec_16(std::string opencl_kernel_name);
    void compute_vec_8(std::string opencl_kernel_name);
    void compute_vec_4(std::string opencl_kernel_name);
    void compute_vec_2(std::string opencl_kernel_name);
    void compute_one_vec_16(std::string opencl_kernel_name);
    void compute_one_vec_8(std::string opencl_kernel_name);
    void compute_one_vec_4(std::string opencl_kernel_name);
    void compute_one_vec_2(std::string opencl_kernel_name);
    void compute_lattice_2d(std::string opencl_kernel_name);

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
    std::size_t vector_size     = 102400000;
    std::size_t iteration_count = 100;

    /*
		Function for fill vector with the test data
		It allows only std::vector of OpenCL Vector Data Types
	*/
    template<typename cl_type>
    void fill(std::vector<cl_type> &vec_a);

    /*
		Function for fill vectors with the test data
		It allows only std::vector of OpenCL Vector Data Types
	*/
    template<typename cl_type>
    void fill(std::vector<cl_type> &vec_a, std::vector<cl_type> &vec_b);

    /*
		Function for compact long vector of type into vector of OpenCL vector

		vec - std::vector<type>, where type is C++ type

		v - std::vector<type>, where type is OpenCL vector type
		Specially aligned types are allowed

		OpenCL Vector Data Types:
			cl_charn
			cl_ucharn
			cl_shortn
			cl_ushortn
			cl_intn
			cl_uintn
			cl_longn
			cl_ulongn
			cl_floatn
			cl_doublen (OPTIONAL)
	*/
    template<typename vec_type, typename cl_type>
    void compact(std::vector<vec_type> const &vec, std::vector<cl_type> &v);

    template<typename iterator_type>
    void _compute(
        std::string opencl_application_name,
        iterator_type start_iterator_a,
        iterator_type end_iterator_a,
        iterator_type start_iterator_b,
        iterator_type end_iterator_b,
        iterator_type start_iterator_c,
        iterator_type end_iterator_c);

    template<typename iterator_type>
    void _compute(std::string opencl_application_name, iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_c, iterator_type end_iterator_c);

    template<typename iterator_type>
    void _compute_lattice_2d(
        std::string opencl_application_name,
        iterator_type start_iterator_a,
        iterator_type end_iterator_a,
        iterator_type start_iterator_b,
        iterator_type end_iterator_b,
        iterator_type start_iterator_c,
        iterator_type end_iterator_c);
};

///////////////////////////////////////////////////////////////////////////////

template<typename cl_type>
void compute_gpu::fill(std::vector<cl_type> &vec_a)
{
    std::size_t cl_type_arr_size = (sizeof(vec_a[0].s) / sizeof(vec_a[0].s[0]));

#pragma omp parallel for
    for(std::size_t i = 0; i < vec_a.size(); i++)
    {
        for(std::size_t n = 0; n < cl_type_arr_size; n++)
        {
            vec_a[i].s[n] = (float)(i + n) / 2;
        }
    }
}

template<typename cl_type>
void compute_gpu::fill(std::vector<cl_type> &vec_a, std::vector<cl_type> &vec_b)
{
    if(vec_a.size() != vec_b.size())
    {
        throw std::length_error("Length error. Vector vec_a is not equal to vec_b");
    }

    std::size_t cl_type_arr_size = (sizeof(vec_a[0].s) / sizeof(vec_a[0].s[0]));

#pragma omp parallel for
    for(std::size_t i = 0; i < vec_a.size(); i++)
    {
        for(std::size_t n = 0; n < cl_type_arr_size; n++)
        {
            vec_a[i].s[n] = (float)(i + n) / 2;
        }
    }

#pragma omp parallel for
    for(std::size_t i = 0; i < vec_b.size(); i++)
    {
        for(std::size_t n = 0; n < cl_type_arr_size; n++)
        {
            vec_b[i].s[n] = (float)(i + n) * 3;
        }
    }
}

template<typename vec_type, typename cl_type>
void compute_gpu::compact(std::vector<vec_type> const &vec, std::vector<cl_type> &v)
{
    std::size_t cl_type_arr_size = (sizeof(v[0].s) / sizeof(v[0].s[0]));

    if((vec.size() / cl_type_arr_size) != v.size())
    {
        throw std::length_error("Length error. Can't compact vec to v.");
    }

#pragma omp parallel for
    for(std::size_t i = 0; i < v.size(); i++)
    {
        for(std::size_t n = 0; n < cl_type_arr_size; n++)
        {
            v[i].s[n] = vec[(i * cl_type_arr_size) + n];
        }
    }
}

template<typename iterator_type>
void compute_gpu::_compute(
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

    std::size_t local_work_group_size = kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());

    cl::CommandQueue queue(context, default_device);

    /*
		We need to specify global (and local) dimensions
		cl::NDRange global(1024);
		cl::NDRange local(64)

		If you don’t specify a local dimension, it is assumed as cl::NullRange, and
		the runtime picks a size for you
	*/
    cl::NDRange global(end_iterator_a - start_iterator_a);
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
void compute_gpu::_compute(
    std::string opencl_application_name, iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_c, iterator_type end_iterator_c)
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

    std::size_t local_work_group_size = kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());

    cl::CommandQueue queue(context, default_device);

    /*
		We need to specify global (and local) dimensions
		cl::NDRange global(1024);
		cl::NDRange local(64)

		If you don’t specify a local dimension, it is assumed as cl::NullRange, and
		the runtime picks a size for you
	*/
    cl::NDRange global(end_iterator_a - start_iterator_a);
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

template<typename iterator_type>
void compute_gpu::_compute_lattice_2d(
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

    std::size_t local_work_group_size = kernel_simple_add.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(cl::Device::getDefault());

    cl::CommandQueue queue(context, default_device);

    /*
		We need to specify global (and local) dimensions
		cl::NDRange global(1024);
		cl::NDRange local(64)

		If you don’t specify a local dimension, it is assumed as cl::NullRange, and
		the runtime picks a size for you
	*/
    //cl::NDRange global(end_iterator_a - start_iterator_a);
    cl::NDRange global((end_iterator_a - start_iterator_a) / 2, 1);
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

#endif // COMPUTE_COMPUTE_GPU_H