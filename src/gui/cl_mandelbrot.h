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
 * @brief Draw OpenCL image (the Mandelbrot set)
 * @author Savelii Pototskii (savalione.com)
 * @author Willem Melching (Author of kernel of the Mandelbrot set)
 * @date 22 Nov 2022
 */
#ifndef GUI_CL_MANDELBROT_H
#define GUI_CL_MANDELBROT_H

#include "gui/gl_image.h"

class cl_mandelbrot : public gl_image
{
private:
    /* SDL2 */
    void init() override;
    void resize_ir(int const &width, int const &height) override;
};

#endif // GUI_CL_MANDELBROT_H