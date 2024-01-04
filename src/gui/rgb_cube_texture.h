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
 * @brief OpenGL RGB textured cube
 * @author Savelii Pototskii (savalione.com)
 * @date 27 Dec 2022
 */
#ifndef GUI_RGB_CUBE_TEXTURE_H
#define GUI_RGB_CUBE_TEXTURE_H

#include "gui/sdl_wrapper.h"

#include <glm/glm.hpp>
#include <array>

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
    GLuint uv_buffer       = 0;
    GLuint matrix_id       = 0;
    GLuint texture         = 0;
    GLuint texture_id      = 0;
    program program_id;
    std::array<GLfloat, 108> vertex_buffer_cube;
    std::array<GLfloat, 108> color_buffer_cube;
    std::array<GLfloat, 108> g_vertex_buffer_cube;
    std::array<GLfloat, 108> g_uv_buffer_cube;

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