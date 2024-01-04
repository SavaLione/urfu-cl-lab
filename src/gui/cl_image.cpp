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
 * @brief Draw OpenCL image
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Nov 2022
 */
#include "gui/cl_image.h"

#include "compute/new_gpu.h"
#include "io/log/logger.h"

std::string kernel_source = R"opencl_kernel(
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

__kernel void draw_image_cl(
    __read_only image2d_t img_in,
    __write_only image2d_t img_out)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    int2 pos = (int2)(x, y);

    uint4 pixel = read_imageui(img_in, sampler, pos);

    if(pixel.x >= 255)
        pixel.x = 0;
    if(pixel.y >= 255)
        pixel.y = 0;
    if(pixel.z >= 255)
        pixel.z = 0;

    pixel.x += 5;
    pixel.y += 10;
    pixel.z += 15;
    pixel.w = 255;

    write_imageui(img_out, pos, pixel);
}
)opencl_kernel";

void cl_image::loop()
{
    if(is_on_focus())
    {
        draw_image_cl(ir, kernel_source);

        /* Update texture */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_window_width(), gl_window_width(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.data());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void cl_image::init()
{
    draw_image_cl(ir, kernel_source);
}

void cl_image::resize_ir(int const &width, int const &height)
{
    ir = image_representation<std::uint8_t>(width, height, 4);
    for(std::size_t i = 0, rgb = 0; i < ir.size(); i++, rgb++)
    {
        switch(rgb)
        {
            case 0:
                ir.data()[i] = 0;
                break;
            case 1:
                ir.data()[i] = 0;
                break;
            case 2:
                ir.data()[i] = 0;
                break;
            case 3:
                ir.data()[i] = 255;
                break;
            default:
                rgb = 0;
                break;
        }
    }

    draw_image_cl(ir, kernel_source);
}