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
 * @brief OpenGL RGB triangle
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Nov 2022
 */
#include "gui/rgb_triangle.h"

#include <SDL2/SDL_keycode.h>
#include <cmath>

#include "io/log/logger.h"

rgb_triangle::rgb_triangle()
{
    /* OpenGL settings */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    resize_window(window_width, window_height);
}

void rgb_triangle::loop()
{
    if(focus)
    {
        std::float_t ratio = (std::float_t)window_width / (std::float_t)window_height;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef((float)rotate * 4.f, 0.f, 0.f, 1.f);

        // clang-format off
        glBegin(GL_TRIANGLES);
            glColor3f(1.f, 0.f, 0.f);
            glVertex3f(-0.6f, -0.4f, 0.f);
            glColor3f(0.f, 1.f, 0.f);
            glVertex3f(0.6f, -0.4f, 0.f);
            glColor3f(0.f, 0.f, 1.f);
            glVertex3f(0.f, 0.6f, 0.f);
        glEnd();
        // clang-format on
    }
}

void rgb_triangle::pool_event()
{
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        _exit = true;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        rotate++;
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        rotate--;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
                break;
            case SDL_WINDOWEVENT:
                switch(event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        spdlog::info("Window resize x: {} y: {}", event.window.data1, event.window.data2);
                        resize_window(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        focus = false;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        focus = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                _exit = true;
                break;
            default:
                break;
        }
    }
}

void rgb_triangle::resize_window(int const &width, int const &height)
{
    window_width  = width;
    window_height = height;

    glViewport(0, 0, window_width, window_height);
}