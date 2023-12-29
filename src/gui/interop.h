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
#ifndef GUI_INTEROP_H
#define GUI_INTEROP_H

#include "gui/sdl_wrapper.h"

#ifdef __WIN32__
    #include <windows.h>
    #include <wingdi.h>
#elif __MINGW32_
    #include <windows.h>
    #include <wingdi.h>
#endif

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
#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>

class interop : public sdl_wrapper
{
public:
    interop();
    ~interop();

private:
    void loop();

    /* SDL2 */
    void resize_window(int const &width, int const &height);

    /* OpenGL */
    GLuint vertex_array_id = 0;
    GLuint vertex_buffer   = 0;
    GLuint gl_texture_;
    program program_id;

    /* OpenCL */
    std::string cl_source;

    boost::compute::device gpu;
    boost::compute::context cl_context;
    boost::compute::command_queue cl_queue;
    boost::compute::program cl_program;
    boost::compute::opengl_texture cl_texture;
};

#endif // GUI_INTEROP_H