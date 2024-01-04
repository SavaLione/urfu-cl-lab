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