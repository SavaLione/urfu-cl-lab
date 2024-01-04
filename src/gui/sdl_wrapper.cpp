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
#include "gui/sdl_wrapper.h"

#include "io/log/logger.h"

sdl_wrapper::sdl_wrapper()
{
    /* SDL 2.0 initialization */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::string _err = "Error initialization SDL2: ";
        _err += SDL_GetError();
        throw std::runtime_error(_err);
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow(_name.c_str(), 0, 0, 1024, 1024, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(!window)
    {
        std::string _err = "Error creating SDL2 window: ";
        _err += SDL_GetError();
        throw std::runtime_error(_err);
    }

    context = SDL_GL_CreateContext(window);
    if(!context)
    {
        std::string _err = "Error creating SDL2 context: ";
        _err += SDL_GetError();
        throw std::runtime_error(_err);
    }

    // Enable glew experimental, this enables some more OpenGL extensions.
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    /* SDL variables */
    SDL_GetWindowSize(window, &window_width, &window_height);
    spdlog::info("Window width: {} height: {}", window_width, window_height);

    // /* Some OpenGL settings */
    // // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // // glClearDepth(1.0f);
    // // glEnable(GL_DEPTH_TEST);
}

sdl_wrapper::~sdl_wrapper()
{
    /* SDL */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_wrapper::pool_event()
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

void sdl_wrapper::loop() {}

void sdl_wrapper::run()
{
    /* Initialization */
    init();

    /* Main loop */
    while(!_exit)
    {
        loop();
        pool_event();

        SDL_GL_SwapWindow(window);
    }
}

void sdl_wrapper::init() {}

void sdl_wrapper::resize_window(int const &width, int const &height)
{
    window_width  = width;
    window_height = height;
}