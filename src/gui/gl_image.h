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
 * @brief Class for drawing 2D image via buffer and OpenGL
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Nov 2022
 */
#ifndef GUI_GL_IMAGE_H
#define GUI_GL_IMAGE_H

#include "gui/sdl_wrapper.h"

#include "gui/image_representation.h"
#include "gui/buffer_representation.h"

#include <array>

class gl_image : sdl_wrapper
{
public:
    gl_image();
    ~gl_image();
    void run();

    /* Fix this */
    bool is_on_focus();
    int gl_window_width();
    int gl_window_height();

protected:
    image_representation<std::uint8_t> ir;

private:
    /* SDL2 */
    void resize_window(int const &width, int const &height);

    virtual void resize_ir(int const &width, int const &height);

    virtual void loop();

    /* OpenGL */
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _ebo = 0;

    std::array<GLuint, 1> textures;
};

#endif // GUI_GL_IMAGE_H