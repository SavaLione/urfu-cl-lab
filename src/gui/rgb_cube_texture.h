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
 * @brief OpenGL RGB textured cube
 * @author Saveliy Pototskiy (SavaLione)
 * @date 27 Dec 2022
 */
#ifndef GUI_RGB_CUBE_TEXTURE_H
#define GUI_RGB_CUBE_TEXTURE_H

#include "gui/sdl_wrapper.h"

#include <glm/glm.hpp>

class rgb_cube_texture : public sdl_wrapper
{
public:
    rgb_cube_texture();
    ~rgb_cube_texture();

private:
    void loop();
    void pool_event();
    void resize_window(int const &width, int const &height);

    /* OpenGL */
    GLuint vertex_array_id = 0;
    GLuint vertex_buffer   = 0;
    GLuint color_buffer    = 0;
    GLuint matrix_id       = 0;
    program program_id;
    std::array<GLfloat, 108> vertex_buffer_cube;
    std::array<GLfloat, 108> color_buffer_cube;

    /* GLM */

    /* Field of view */
    std::float_t fov = 45.0f;

    std::float_t ratio_x = 4.0f;
    std::float_t ratio_y = 3.0f;

    std::float_t display_range_near = 0.1f;
    std::float_t display_range_far  = 100.0f;

    /* Projection matrix */
    glm::mat4 projection;

    /* Camera matrix */
    glm::mat4 view;

    /* Model matrix */
    glm::mat4 model;

    /* Model view projection */
    glm::mat4 mvp;
};

#endif // GUI_RGB_CUBE_TEXTURE_H