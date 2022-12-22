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
 * @brief Interoperability between OpenCL and OpenGL
 * @author Saveliy Pototskiy (SavaLione)
 * @date 20 Dec 2022
 */
#include "gui/interop.h"

#include <GL/glx.h>
#include <cstddef>
#include <string>
#include <CL/cl.h>
#include <CL/cl_gl.h>

#include <boost/compute/event.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/async/future.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/interop/opengl.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include <boost/compute/command_queue.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/program.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/interop/opengl.hpp>
#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>

#include "io/log/logger.h"

/* OpenCL */
boost::compute::context context_;
boost::compute::command_queue queue_;
boost::compute::program program_;
GLuint gl_texture_;
boost::compute::opengl_texture cl_texture_;

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
    try
    {
        context_ = boost::compute::opengl_create_shared_context();
    }
    catch(std::exception &e)
    {
        spdlog::error("Can't create OpenGL shared context: {}", e.what());
    }

    // get gpu device
    boost::compute::device gpu = context_.get_device();
    spdlog::info("device: {}", gpu.name());

    // setup command queue
    queue_ = boost::compute::command_queue(context_, gpu);

    // build mandelbrot program
    program_ = boost::compute::program::create_with_source(cl_source, context_);
    program_.build();

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
    cl_texture_ = boost::compute::opengl_texture(context_, GL_TEXTURE_2D, 0, gl_texture_, CL_MEM_WRITE_ONLY);

    // // create the OpenGL/OpenCL shared context
    // try
    // {
    //     context_ = boost::compute::opengl_create_shared_context();
    // }
    // catch(std::exception &e)
    // {
    //     spdlog::error("Can't create OpenGL shared context: {}", e.what());
    // }

    // boost::compute::device gpu = context_.get_device();
    // spdlog::info("device: {}", gpu.name());

    /* OpenCL */
    // // get the default device
    // boost::compute::device gpu = boost::compute::system::default_device();

    // // create context object
    // boost::compute::context ctx = boost::compute::system::default_context();

    // query number of devices using the OpenCL API
    // cl_uint num_devices;
    // clGetContextInfo(ctx, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &num_devices, 0);
    // spdlog::info("num_devices: {}", num_devices);

    /* OpenGL */
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // glGenVertexArrays(1, &vertex_array_id);
    // glBindVertexArray(vertex_array_id);

    // // Create and compile program
    // program_id = program(vertex_shader, fragment_shader);

    // // Triangle
    // static const GLfloat g_vertex_buffer_data[] = {
    //     -1.0f,
    //     -1.0f,
    //     0.0f,
    //     1.0f,
    //     -1.0f,
    //     0.0f,
    //     0.0f,
    //     1.0f,
    //     0.0f,
    // };

    // // Vertex buffer
    // glGenBuffers(1, &vertex_buffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, 0.0, window_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // setup the mandelbrot kernel
    boost::compute::kernel kernel(program_, "mandelbrot");
    kernel.set_arg(0, cl_texture_);

    // acquire the opengl texture so it can be used in opencl
    boost::compute::opengl_enqueue_acquire_gl_objects(1, &cl_texture_.get(), queue_);

    // execute the mandelbrot kernel
    queue_.enqueue_nd_range_kernel(kernel, boost::compute::dim(0, 0), boost::compute::dim(window_width, window_height), boost::compute::dim(1, 1));

    // release the opengl texture so it can be used by opengl
    boost::compute::opengl_enqueue_release_gl_objects(1, &cl_texture_.get(), queue_);

    // ensure opencl is finished before rendering in opengl
    queue_.finish();

    // // draw a single quad with the mandelbrot image texture
    // glEnable(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, gl_texture_);

    // // clang-format off
    // glBegin(GL_QUADS);
    // glTexCoord2f(0, 0); glVertex2f(0, 0);
    // glTexCoord2f(0, 1); glVertex2f(0, window_height);
    // glTexCoord2f(1, 1); glVertex2f(window_width, window_height);
    // glTexCoord2f(1, 0); glVertex2f(window_width, 0);
    // glEnd();
    // // clang-format on
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // // Use our shader
    // glUseProgram(program_id.id());

    // // First attribute buffer : vertices
    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // /*
    //     0           - index
    //     3           - size
    //     GL_FLOAT    - type
    //     GL_FALSE    - normalized
    //     0           - stride
    //     (void *)0   - array buffer offset
    // */
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // // Draw the triangle
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // glDisableVertexAttribArray(0);

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