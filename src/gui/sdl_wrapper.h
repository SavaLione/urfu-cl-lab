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
 * @brief SDL2 wrapper
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Nov 2022
 */
#ifndef GUI_SDL_WRAPPER_H
#define GUI_SDL_WRAPPER_H

// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include <string>

#include "gui/program.h"

class sdl_wrapper
{
public:
    sdl_wrapper();
    ~sdl_wrapper();
    virtual void run();

protected:
    virtual void pool_event();
    virtual void loop();
    virtual void init();

    /* SDL */
    SDL_Window *window;
    SDL_GLContext context;
    SDL_Event event;

    // Is in focus?
    bool focus = false;

    virtual void resize_window(int const &width, int const &height);

    int window_width  = 0;
    int window_height = 0;
    bool _exit        = false;
    std::string _name = "nyx";

    std::string fragment_shader;
    std::string vertex_shader;
};

#endif // GUI_SDL_WRAPPER_H