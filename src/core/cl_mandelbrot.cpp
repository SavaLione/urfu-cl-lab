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
 * @brief Draw OpenCL image (the Mandelbrot set)
 * @author Saveliy Pototskiy (SavaLione)
 * @author Willem Melching (Author of kernel of the Mandelbrot set)
 * @date 22 Nov 2022
 */
#include "core/cl_mandelbrot.h"

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
        case 0: return (uint3)(66, 30, 15);
        case 1: return (uint3)(25, 7, 26);
        case 2: return (uint3)(9, 1, 47);
        case 3: return (uint3)(4, 4, 73);
        case 4: return (uint3)(0, 7, 100);
        case 5: return (uint3)(12, 44, 138);
        case 6: return (uint3)(24, 82, 177);
        case 7: return (uint3)(57, 125, 209);
        case 8: return (uint3)(134, 181, 229);
        case 9: return (uint3)(211, 236, 248);
        case 10: return(uint3)(241, 233, 191);
        case 11: return(uint3)(248, 201, 95);
        case 12: return(uint3)(255, 170, 0);
        case 13: return(uint3)(204, 128, 0);
        case 14: return(uint3)(153, 87, 0);
        case 15: return(uint3)(106, 52, 3);
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