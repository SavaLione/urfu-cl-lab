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
 * @brief OpenCL particles
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Dec 2022
 */
#include "gui/cl_particles.h"

#include "io/log/logger.h"

cl_particles::cl_particles()
{
    /* OpenCL */
    cl_source = R"cl(
        __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

        __kernel void mandelbrot(
            __write_only image2d_t image_out,
            __read_only image2d_t image_in,
            __write_only image2d_t image_out_to_host)
        {
            const uint x_coord = get_global_id(0);
            const uint y_coord = get_global_id(1);
            int2 position = (int2)(x_coord, y_coord);

            const uint width = get_global_size(0);
            const uint height = get_global_size(1);

            float4 pixel = read_imagef(image_in, sampler, position);

            // if(pixel.x >= 255)
            //     pixel.x = 0;
            // if(pixel.y >= 255)
            //     pixel.y = 0;
            // if(pixel.z >= 255)
            //     pixel.z = 0;
            // pixel.w = 255;

            // pixel.x += 20;
            // pixel.y += 50;
            // pixel.z += 20;

            write_imagef(image_out, position, pixel);
            write_imagef(image_out_to_host, position, pixel);
        };
    )cl";

    /* OpenGL */
    // setup opengl
    glDisable(GL_LIGHTING);

    /* OpenCL */

    // create the OpenGL/OpenCL shared context
    cl_context = boost::compute::opengl_create_shared_context();

    // get gpu device
    gpu = cl_context.get_device();
    spdlog::info("device: {}", gpu.name());

    // setup command queue
    cl_queue = boost::compute::command_queue(cl_context, gpu);

    // build mandelbrot program
    cl_program = boost::compute::program::create_with_source(cl_source, cl_context);
    try
    {
        cl_program.build();
    }
    catch(std::exception const &e)
    {
        spdlog::error("OpenCL build error: {}", e.what());
        spdlog::error("OpenCL build log: \n{}", cl_program.build_log());
    }
    catch(...)
    {
        spdlog::error("OpenCL unexpected build error");
    }

    // Resize
    resize_window(window_width, window_height);
}

cl_particles::~cl_particles()
{
    // Cleanup VBO
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
}

void cl_particles::loop()
{
    for(std::size_t i = 0, n = 0; i < ir.size(); i++, n++)
    {
        if(n >= 4)
            n = 0;
        switch(n)
        {
            case 0:
                ir.data()[i] += 20;
                break;
            case 1:
                ir.data()[i] += 50;
                break;
            case 2:
                ir.data()[i] += 20;
                break;
            case 3:
                ir.data()[i] = 255;
                break;
            default:
                break;
        }
    }

    // if(focus)
    // {
    // for(std::size_t i = 0; i < ir.size(); i++)
    //     ir.data()[i] += 1;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Paint OpenGL */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, 0.0, window_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // setup the mandelbrot kernel
    boost::compute::kernel kernel(cl_program, "mandelbrot");
    kernel.set_arg(0, cl_texture);
    kernel.set_arg(1, cl_image_in);
    kernel.set_arg(2, cl_image_out);

    // acquire the opengl texture so it can be used in opencl
    boost::compute::opengl_enqueue_acquire_gl_objects(1, &cl_texture.get(), cl_queue);

    // write buffer
    cl_queue.enqueue_write_image(cl_image_in, cl_image_in.origin(), cl_image_in.size(), ir.const_data());
    cl_queue.finish();

    // execute the mandelbrot kernel
    cl_queue.enqueue_nd_range_kernel(kernel, boost::compute::dim(0, 0), boost::compute::dim(window_width, window_height), boost::compute::dim(1, 1));

    // release the opengl texture so it can be used by opengl
    boost::compute::opengl_enqueue_release_gl_objects(1, &cl_texture.get(), cl_queue);

    // ensure opencl is finished before rendering in opengl
    cl_queue.finish();

    // read data from device
    std::size_t const width_const  = window_width;
    std::size_t const height_const = window_height;
    std::size_t origin[3]          = {0, 0, 0};
    std::size_t region[3]          = {width_const, height_const, 1};
    cl_queue.enqueue_read_image(cl_image_out, origin, region, 0, 0, ir.data());
    cl_queue.finish();

    // draw a single quad with the mandelbrot image texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gl_texture_);

    // clang-format off
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(0, 1); glVertex2f(0, window_height);
        glTexCoord2f(1, 1); glVertex2f(window_width, window_height);
        glTexCoord2f(1, 0); glVertex2f(window_width, 0);
        glEnd();
    // clang-format on
    // }
}

void cl_particles::resize_window(int const &width, int const &height)
{
    /* Set window size */
    window_width  = width;
    window_height = height;

    ir = image_representation<float>(window_width, window_height, 4);
    ir.fill_zeros();

    /* OpenGL resize */
    // resize viewport
    glViewport(0, 0, window_width, window_height);

    // delete old texture
    if(gl_texture_)
    {
        glDeleteTextures(1, &gl_texture_);
        gl_texture_ = 0;
    }

    // generate new texture
    glGenTextures(1, &gl_texture_);
    glBindTexture(GL_TEXTURE_2D, gl_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // create opencl object for the texture
    cl_texture = boost::compute::opengl_texture(cl_context, GL_TEXTURE_2D, 0, gl_texture_, CL_MEM_WRITE_ONLY);

    // create in image
    cl_image_in = boost::compute::image2d(cl_context, window_width, window_height, boost::compute::image_format(CL_RGBA, CL_FLOAT), boost::compute::image2d::read_only);

    // create out image
    cl_image_out = boost::compute::image2d(cl_context, window_width, window_height, boost::compute::image_format(CL_RGBA, CL_FLOAT), boost::compute::image2d::write_only);
}