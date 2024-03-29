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
 * @brief Interoperability between OpenCL and OpenGL
 * @author Savelii Pototskii (savalione.com)
 * @date 20 Dec 2022
 */
#include "gui/interop.h"

#include "io/log/logger.h"

interop::interop()
{
    /* Shaders */
    fragment_shader = R"shader(
        #version 330 core

        // Ouput data
        out vec3 color;

        void main()
        {
	        // Output color = red 
	        color = vec3(1,0,0);
        }
    )shader";

    vertex_shader = R"shader(
        #version 330 core

        // Input vertex data, different for all executions of this shader.
        layout(location = 0) in vec3 vertexPosition_modelspace;

        void main()
        {
            gl_Position.xyz = vertexPosition_modelspace;
            gl_Position.w = 1.0;
        }
    )shader";

    /* OpenCL */
    cl_source = R"cl(
        // map value to color
        float4 color(uint i)
        {
            uchar c = i;
            uchar x = 35;
            uchar y = 25;
            uchar z = 15;
            uchar max = 255;

            if(i == 256)
                return (float4)(0, 0, 0, 255);
            else
                return (float4)(max-x*i, max-y*i, max-z*i, max) / 255.f;
        }

        __kernel void mandelbrot(__write_only image2d_t image)
        {
            const uint x_coord = get_global_id(0);
            const uint y_coord = get_global_id(1);
            const uint width = get_global_size(0);
            const uint height = get_global_size(1);

            float x_origin = ((float) x_coord / width) * 3.25f - 2.0f;
            float y_origin = ((float) y_coord / height) * 2.5f - 1.25f;

            float x = 0.0f;
            float y = 0.0f;

            uint i = 0;
            while(x*x + y*y <= 4.f && i < 256)
            {
                float tmp = x*x - y*y + x_origin;
                y = 2*x*y + y_origin;
                x = tmp;
                i++;
            }

            int2 coord = { x_coord, y_coord };
            write_imagef(image, coord, color(i));
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
    cl_program.build();

    // Resize
    resize_window(window_width, window_height);
}

interop::~interop()
{
    // Cleanup VBO
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
}

void interop::loop()
{
    if(focus)
    {
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

        // acquire the opengl texture so it can be used in opencl
        boost::compute::opengl_enqueue_acquire_gl_objects(1, &cl_texture.get(), cl_queue);

        // execute the mandelbrot kernel
        cl_queue.enqueue_nd_range_kernel(kernel, boost::compute::dim(0, 0), boost::compute::dim(window_width, window_height), boost::compute::dim(1, 1));

        // release the opengl texture so it can be used by opengl
        boost::compute::opengl_enqueue_release_gl_objects(1, &cl_texture.get(), cl_queue);

        // ensure opencl is finished before rendering in opengl
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
    }
}

void interop::resize_window(int const &width, int const &height)
{
    /* Set window size */
    window_width  = width;
    window_height = height;

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
}