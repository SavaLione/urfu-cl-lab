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
 * @brief OpenGL vao triangle
 * @author Savelii Pototskii (savalione.com)
 * @date 27 Nov 2022
 */
#include "gui/vao_triangle.h"

#include <cstddef>
#include <string>

#include "io/log/logger.h"

vao_triangle::vao_triangle()
{
    /* Shaders */
    fragment_shader = R"shader(
        #version 330 core

        // Ouput data
        out vec3 color;

        void main()
        {
	        // Output color = red 
	        color = vec3(1,0,0);
        }
    )shader";

    vertex_shader = R"shader(
        #version 330 core

        // Input vertex data, different for all executions of this shader.
        layout(location = 0) in vec3 vertexPosition_modelspace;

        void main()
        {
            gl_Position.xyz = vertexPosition_modelspace;
            gl_Position.w = 1.0;
        }
    )shader";

    /* OpenGL */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    resize_window(window_width, window_height);
}

vao_triangle::~vao_triangle()
{
    // Cleanup VBO
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
}

void vao_triangle::loop()
{
    if(focus)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(program_id.id());

        // First attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        /*
            0           - index
            3           - size
            GL_FLOAT    - type
            GL_FALSE    - normalized
            0           - stride
            (void *)0   - array buffer offset
        */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(0);
    }
}

void vao_triangle::resize_window(int const &width, int const &height)
{
    window_width  = width;
    window_height = height;

    /* Clean up */
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);

    glViewport(0, 0, window_width, window_height);

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Create and compile program
    program_id = program(vertex_shader, fragment_shader);

    // Triangle
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,
        -1.0f,
        0.0f,
        1.0f,
        -1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    // Vertex buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}