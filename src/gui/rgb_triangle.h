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
#ifndef GUI_RGB_TRIANGLE_H
#define GUI_RGB_TRIANGLE_H

#include "gui/sdl_wrapper.h"

#include <cstdint>

class rgb_triangle : public sdl_wrapper
{
public:
    rgb_triangle();

private:
    /* SDL2 */
    void loop();
    void pool_event();
    void resize_window(int const &width, int const &height);

    /* RGB OpenGL triangle */
    std::int8_t rotate = 0;
};

#endif // GUI_RGB_TRIANGLE_H