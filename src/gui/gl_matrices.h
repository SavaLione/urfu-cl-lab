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
 * @brief OpenGL operations with matrices
 * @author Savelii Pototskii (savalione.com)
 * @date 16 Dec 2022
 */
#ifndef GUI_GL_MATRICES_H
#define GUI_GL_MATRICES_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gui/sdl_wrapper.h"
#include "gui/program.h"

class gl_matrices : public sdl_wrapper
{
public:
    gl_matrices();
    ~gl_matrices();

private:
    void loop();
    void pool_event();

    /* OpenGL */
    GLuint vertex_array_id = 0;
    GLuint vertex_buffer   = 0;
    GLuint matrix_id       = 0;
    program program_id;

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

#endif // GUI_GL_MATRICES_H