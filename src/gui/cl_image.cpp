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
 * @brief Draw OpenCL image
 * @author Saveliy Pototskiy (SavaLione)
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