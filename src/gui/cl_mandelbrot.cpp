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
#include "gui/cl_mandelbrot.h"

#include "compute/new_gpu.h"

#include "io/log/logger.h"

/*
    Author of kernel: Willem Melching
    https://blog.willemmelching.nl/random/2020/04/12/mandelbrot/
*/
std::string mandelbrot_kernel_source = R"opencl_kernel(
uint3 get_color(uint steps)
{
    steps = steps % 16;

    switch (steps)
    {
        case 0:  return (uint3)(66,  30,  15);
        case 1:  return (uint3)(25,  7,   26);
        case 2:  return (uint3)(9,   1,   47);
        case 3:  return (uint3)(4,   4,   73);
        case 4:  return (uint3)(0,   7,   100);
        case 5:  return (uint3)(12,  44,  138);
        case 6:  return (uint3)(24,  82,  177);
        case 7:  return (uint3)(57,  125, 209);
        case 8:  return (uint3)(134, 181, 229);
        case 9:  return (uint3)(211, 236, 248);
        case 10: return (uint3)(241, 233, 191);
        case 11: return (uint3)(248, 201, 95);
        case 12: return (uint3)(255, 170, 0);
        case 13: return (uint3)(204, 128, 0);
        case 14: return (uint3)(153, 87,  0);
        case 15: return (uint3)(106, 52,  3);
    }

    return (uint3)(0, 0, 0);
}

__kernel void draw_write_only_cl(
    __write_only image2d_t out)
{
    int2 pos = (int2)(get_global_id(0), get_global_id(1));
    int2 size = get_image_dim(out);

    // Scale x to -2.5 to 1
    float x0 = (float)pos.x / size.x;
    x0 = x0*3.25 - 2;

    // Scale y to -1 to 1
    float y0 = (float)pos.y / size.y;
    y0 = y0*2.0 - 1.0;

    float x = 0.0;
    float y = 0.0;

    uint max_its = 256;
    uint i = 0;
    float d = 0.0;

    while (i < max_its && d < 4.0)
    {
        float xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;

        d = x*x + y*y;
        i++;
    }

    uint4 color = (255, 255, 255, 255);

    if (d < 4.0)
    {
        color.xyz = (uint3)(0, 0, 0);
    }
    else
    {
        color.xyz = get_color(i);
    }

    write_imageui(out, pos, color);
}
)opencl_kernel";

void cl_mandelbrot::init()
{
    draw_write_only_cl(ir, mandelbrot_kernel_source);
}

void cl_mandelbrot::resize_ir(int const &width, int const &height)
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

    draw_write_only_cl(ir, mandelbrot_kernel_source);

    spdlog::info("ir width: {} height: {}", ir.width(), ir.height());
}