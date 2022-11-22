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
 * @brief New gpu functions and etc.
 * @author Saveliy Pototskiy (SavaLione)
 * @date 22 Nov 2022
 */
#include "core/new_gpu.h"

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

#include <CL/opencl.hpp>

#define BOOST_COMPUTE_CL_VERSION_1_2

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/async/future.hpp>
#include <boost/compute/command_queue.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/event.hpp>
#include <boost/compute/interop/opengl.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/program.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>

namespace compute = boost::compute;

#include <spdlog/spdlog.h>

std::string image_cl_kernel_source = R"opencl_kernel(
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

__kernel void draw_image_cl(
    __read_only image2d_t img_in,
    __write_only image2d_t img_out)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    int2 pos = (int2)(x, y);

    uint4 pixel = read_imageui(img_in, sampler, pos);
    write_imageui(img_out, pos, pixel);
}
)opencl_kernel";

std::string draw_image_buffer_cl_kernel_source = R"opencl_kernel(
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

__kernel void draw_image_buffer_cl(
    __read_only image2d_t img_in,
    __write_only image2d_t img_out,
    __global const uchar4 *elements_in,
    __global uchar4 *elements_out)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    int2 pos = (int2)(x, y);

    const int2 image_size = (int2)(get_image_width(img_in), get_image_height(img_in));

    uint4 pixel = read_imageui(img_in, sampler, pos);
    uint4 pixel_white = (uint4)(255, 255, 255, 255);
    uint4 pixel_black = (uint4)(0, 0, 0, 0);

    uchar LIFETIME = 5;

    if(pixel.s0 != 0)
    {
        int element_num = y*image_size.x + x;
        uchar4 element = elements_in[element_num];

        // lifetime
        if(element.s0 != 0)
        {
            // write pixel and area
            write_imageui(img_out, pos, pixel_white);
            
            write_imageui(img_out, (int2)(pos.x + 1, pos.y + 0), pixel_white);
            write_imageui(img_out, (int2)(pos.x + 0, pos.y + 1), pixel_white);
            write_imageui(img_out, (int2)(pos.x + 1, pos.y + 1), pixel_white);
            write_imageui(img_out, (int2)(pos.x - 1, pos.y + 0), pixel_white);
            write_imageui(img_out, (int2)(pos.x + 0, pos.y - 1), pixel_white);
            write_imageui(img_out, (int2)(pos.x - 1, pos.y - 1), pixel_white);
            write_imageui(img_out, (int2)(pos.x + 1, pos.y - 1), pixel_white);
            write_imageui(img_out, (int2)(pos.x - 1, pos.y + 1), pixel_white);

            element.s0--;

            // direction 0
            if(element.s1 == 0)
            {
                if(element_num >= 8)
                {
                    if(element_num <= ((image_size.x * image_size.y) - 8))
                    {
                        // write_imageui(img_out, (int2)(pos.x - 1, pos.y - 1), pixel_white);
                        // elements_out[element_num - 4].s0 += LIFETIME;
                        // elements_out[element_num - 4].s1 = 0;

                        for(int i = 0; i < 8; i++)
                        {
                            elements_out[element_num - i].s0 += LIFETIME;
                            elements_out[element_num - i].s1 = 0;
                        }
                    }
                }
            }

            // Write changes
            elements_out[element_num] = element;
        }
    }
}
)opencl_kernel";

void draw_image_cl(image_representation &img)
{
    // get the default device
    compute::device device = compute::system::default_device();

    // create a context for the device
    compute::context context(device);

    // image2d format
    compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

    // build program
    compute::program program = compute::program::create_with_source(image_cl_kernel_source, context);

    try
    {
        program.build();
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL build error: {}", e.what());
        spdlog::error("OpenCL build log: \n{}", program.build_log());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected build error");
    }

    // setup kernel
    compute::kernel kernel(program, "draw_image_cl");

    // create input and output images on the gpu
    compute::image2d img_2d_in(context, img.width(), img.height(), format, compute::image2d::read_only);
    compute::image2d img_2d_out(context, img.width(), img.height(), format, compute::image2d::write_only);

    // set args
    try
    {
        kernel.set_arg(0, img_2d_in);
        kernel.set_arg(1, img_2d_out);
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL set_arg error: {}", e.what());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected set_arg error");
    }

    // regions
    std::size_t origin[3] = {0, 0, 0};
    std::size_t region[3] = {img.width(), img.height(), 1};

    // create a command queue
    compute::command_queue queue(context, device);

    // write buffers
    queue.enqueue_write_image(img_2d_in, img_2d_in.origin(), img_2d_in.size(), img.const_data());
    queue.finish();

    // compute
    queue.enqueue_nd_range_kernel(kernel, 2, origin, region, 0);
    queue.finish();

    // read data from device
    queue.enqueue_read_image(img_2d_out, origin, region, 0, 0, img.data());
    queue.finish();
}

void draw_image_cl(image_representation &img, std::string const &kern)
{
    // get the default device
    compute::device device = compute::system::default_device();

    // create a context for the device
    compute::context context(device);

    // image2d format
    compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

    // build program
    compute::program program = compute::program::create_with_source(kern, context);

    try
    {
        program.build();
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL build error: {}", e.what());
        spdlog::error("OpenCL build log: \n{}", program.build_log());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected build error");
    }

    // setup kernel
    compute::kernel kernel(program, "draw_image_cl");

    // create input and output images on the gpu
    compute::image2d img_2d_in(context, img.width(), img.height(), format, compute::image2d::read_only);
    compute::image2d img_2d_out(context, img.width(), img.height(), format, compute::image2d::write_only);

    // set args
    try
    {
        kernel.set_arg(0, img_2d_in);
        kernel.set_arg(1, img_2d_out);
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL set_arg error: {}", e.what());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected set_arg error");
    }

    // regions
    std::size_t origin[3] = {0, 0, 0};
    std::size_t region[3] = {img.width(), img.height(), 1};

    // create a command queue
    compute::command_queue queue(context, device);

    // write buffers
    queue.enqueue_write_image(img_2d_in, img_2d_in.origin(), img_2d_in.size(), img.const_data());
    queue.finish();

    // compute
    queue.enqueue_nd_range_kernel(kernel, 2, origin, region, 0);
    queue.finish();

    // read data from device
    queue.enqueue_read_image(img_2d_out, origin, region, 0, 0, img.data());
    queue.finish();
}

void draw_image_buffer_cl(image_representation &img, buffer_representation<boost::compute::uchar4_> &buff)
{
    // get the default device
    compute::device device = compute::system::default_device();

    // create a context for the device
    compute::context context(device);

    // image2d format
    compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

    // build program
    compute::program program = compute::program::create_with_source(draw_image_buffer_cl_kernel_source, context);

    try
    {
        program.build();
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL build error: {}", e.what());
        spdlog::error("OpenCL build log: \n{}", program.build_log());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected build error");
    }

    // setup kernel
    compute::kernel kernel_cl_click_map(program, "new_test_kernel");

    // create input and output images on the gpu
    compute::image2d img_2d_in(context, img.width(), img.height(), format, compute::image2d::read_only);
    compute::image2d img_2d_out(context, img.width(), img.height(), format, compute::image2d::write_only);
    compute::buffer buffer_ipr_in(context, buff.size() * sizeof(compute::uchar4_), compute::buffer::read_only);
    compute::buffer buffer_ipr_out(context, buff.size() * sizeof(compute::uchar4_), compute::buffer::write_only);

    // set args
    try
    {
        kernel_cl_click_map.set_arg(0, img_2d_in);
        kernel_cl_click_map.set_arg(1, img_2d_out);
        kernel_cl_click_map.set_arg(2, buffer_ipr_in);
        kernel_cl_click_map.set_arg(3, buffer_ipr_out);
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL set_arg error: {}", e.what());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected set_arg error");
    }

    // regions
    std::size_t origin[3] = {0, 0, 0};
    std::size_t region[3] = {img.width(), img.height(), 1};

    // create a command queue
    compute::command_queue queue(context, device);

    // write buffers
    queue.enqueue_write_image(img_2d_in, img_2d_in.origin(), img_2d_in.size(), img.const_data());
    queue.enqueue_write_buffer(buffer_ipr_in, 0, buff.size() * sizeof(compute::uchar4_), buff.const_data());
    queue.finish();

    // compute
    queue.enqueue_nd_range_kernel(kernel_cl_click_map, 2, origin, region, 0);
    queue.finish();

    // read data from device
    queue.enqueue_read_image(img_2d_out, origin, region, 0, 0, img.data());
    queue.enqueue_read_buffer(buffer_ipr_out, 0, buff.size() * sizeof(compute::uchar4_), buff.data());
    queue.finish();
}

void draw_write_only_cl(image_representation &img, std::string const &kern)
{
    // get the default device
    compute::device device = compute::system::default_device();

    // create a context for the device
    compute::context context(device);

    // image2d format
    compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);

    // build program
    compute::program program = compute::program::create_with_source(kern, context);

    try
    {
        program.build();
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL build error: {}", e.what());
        spdlog::error("OpenCL build log: \n{}", program.build_log());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected build error");
    }

    // setup kernel
    compute::kernel kernel(program, "draw_write_only_cl");

    // create input and output images on the gpu
    compute::image2d img_2d_out(context, img.width(), img.height(), format, compute::image2d::write_only);

    // set args
    try
    {
        kernel.set_arg(0, img_2d_out);
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL set_arg error: {}", e.what());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected set_arg error");
    }

    // regions
    std::size_t origin[3] = {0, 0, 0};
    std::size_t region[3] = {img.width(), img.height(), 1};

    // create a command queue
    compute::command_queue queue(context, device);

    // compute
    queue.enqueue_nd_range_kernel(kernel, 2, origin, region, 0);
    queue.finish();

    // read data from device
    queue.enqueue_read_image(img_2d_out, origin, region, 0, 0, img.data());
    queue.finish();
}