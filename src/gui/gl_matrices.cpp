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
#include "gui/gl_matrices.h"

#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>

#include <SDL2/SDL_keycode.h>

#include "gui/program.h"
#include "io/log/logger.h"

gl_matrices::gl_matrices()
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

        // Values that stay constant for the whole mesh.
        uniform mat4 MVP;

        void main()
        {
        	// Output position of the vertex, in clip space : MVP * position
	        gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

        }
    )shader";

    /* OpenGL */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

    /* GLM */
    matrix_id = glGetUniformLocation(program_id.id(), "MVP");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projection = glm::perspective(glm::radians(fov), ratio_x / ratio_y, display_range_near, display_range_far);

    // Camera matrix
    view = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    mvp = projection * view * model; // Remember, matrix multiplication is the other way around

    // Vertex buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}

gl_matrices::~gl_matrices()
{
    // Cleanup VBO
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
}

void gl_matrices::loop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id.id());

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

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

void gl_matrices::pool_event()
{
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        _exit = true;
                        break;
                    case SDLK_f:
                        fov += 5.0f;
                        projection = glm::perspective(glm::radians(fov), ratio_x / ratio_y, display_range_near, display_range_far);

                        mvp = projection * view * model;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                spdlog::info("Touch x: {} y: {}", event.button.x, event.button.y);
                break;
            case SDL_QUIT:
                _exit = true;
                break;
            default:
                break;
        }
    }
}