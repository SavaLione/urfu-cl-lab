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
 * @brief OpenGL RGB cube
 * @author Savelii Pototskii (savalione.com)
 * @date 27 Dec 2022
 */
#include "gui/rgb_cube.h"

#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iterator>
#include <string>
#include <type_traits>
#include <SDL2/SDL_keycode.h>

#include "gui/program.h"
#include "io/log/logger.h"

rgb_cube::rgb_cube()
{
    /* Shaders */
    fragment_shader = R"shader(
        #version 330 core

        // Interpolated values from the vertex shaders
        in vec3 fragmentColor;

        // Ouput data
        out vec3 color;

        void main()
        {
            // Output color = color specified in the vertex shader, interpolated between all 3 surrounding vertices
            color = fragmentColor;
        }
    )shader";

    vertex_shader = R"shader(
        #version 330 core

        // Input vertex data, different for all executions of this shader.
        layout(location = 0) in vec3 vertexPosition_modelspace;
        layout(location = 1) in vec3 vertexColor;

        // Output data ; will be interpolated for each fragment.
        out vec3 fragmentColor;

        // Values that stay constant for the whole mesh.
        uniform mat4 MVP;

        void main()
        {	
            // Output position of the vertex, in clip space : MVP * position
            gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

            // The color of each vertex will be interpolated to produce the color of each fragment
            fragmentColor = vertexColor;
        }
    )shader";

    /* OpenGL cube */
    // clang-format off
    vertex_buffer_cube =
    {
        // triangle 1
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        // triangle 2
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

        // triangle 3
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,

        // triangle 4
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        // triangle 5
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,

        // triangle 6
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,

        // triangle 7
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,

        // triangle 8
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,

        // triangle 9
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,

        // triangle 10
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

        // triangle 11
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,

        // triangle 12
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    /* OpenGL colors for cube */
    color_buffer_cube =
    {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    // clang-format on

    /* OpenGL */
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Create and compile program
    program_id = program(vertex_shader, fragment_shader);

    /* GLM */
    matrix_id = glGetUniformLocation(program_id.id(), "MVP");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projection = glm::perspective(glm::radians(fov), ratio_x / ratio_y, display_range_near, display_range_far);

    // Camera matrix
    view = glm::lookAt(
        glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
        glm::vec3(0, 0, 0),  // and looks at the origin
        glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    model = glm::mat4(1.0f);

    // Our ModelViewProjection : multiplication of our 3 matrices
    mvp = projection * view * model;

    resize_window(window_width, window_height);
}

void rgb_cube::resize_window(int const &width, int const &height)
{
    window_width  = width;
    window_height = height;

    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(color_buffer != 0)
        glDeleteBuffers(1, &color_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Vertex buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_cube), vertex_buffer_cube.data(), GL_STATIC_DRAW);

    // Color buffer
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_cube), color_buffer_cube.data(), GL_STATIC_DRAW);
}

rgb_cube::~rgb_cube()
{
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(color_buffer != 0)
        glDeleteBuffers(1, &color_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
}

void rgb_cube::loop()
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

    // Color buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    /*
        1           - attribute (layout in the shader)
        3           - size
        GL_FLOAT    - type
        GL_FALSE    - normalized
        0           - stride
        (void *)0   - array buffer offset
    */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // Draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void rgb_cube::pool_event()
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