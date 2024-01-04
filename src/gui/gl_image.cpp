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
 * @brief Class for drawing 2D image via buffer and OpenGL
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Nov 2022
 */
#include "gui/gl_image.h"

#include "gui/shader.h"
#include "io/log/logger.h"

gl_image::gl_image()
{
    /* Shaders */
    vertex_shader = R"glsl(
    #version 150 core
    in vec2 position;
    in vec3 color;
    in vec2 texcoord;
    out vec3 Color;
    out vec2 Texcoord;
    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

    fragment_shader = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D tex_sampler;
    void main()
    {
        outColor = texture(tex_sampler, Texcoord);
    }
)glsl";

    /* OpenGL settings */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    resize_window(window_width, window_height);
}

gl_image::~gl_image()
{
    /* OpenGL */
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteTextures(1, textures.data());
}

void gl_image::run()
{
    /* Initialization */
    init();

    /* Main loop */
    while(!_exit)
    {
        pool_event();

        loop();

        SDL_GL_SwapWindow(window);
    }
}

void gl_image::resize_window(int const &width, int const &height)
{
    window_width  = width;
    window_height = height;

    /* OpenGL */
    if(_ebo != 0)
        glDeleteBuffers(1, &_ebo);
    if(_vbo != 0)
        glDeleteBuffers(1, &_vbo);
    if(_vao != 0)
        glDeleteVertexArrays(1, &_vao);
    if(textures[0] != 0)
        glDeleteTextures(1, textures.data());
    for(std::size_t i = 0; i < textures.size(); i++)
        textures[i] = 0;

    // resize viewport
    glViewport(0, 0, window_width, window_height);

    // Create Vertex Array Object
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &_vbo);

    GLfloat vertices[] = {
        //  Position      Color             Texcoords
        -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
        1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
        1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    glGenBuffers(1, &_ebo);

    GLuint elements[] = {0, 1, 2, 2, 3, 0};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    shader vert_shader(::shader_type::vertex, vertex_shader);

    // Create and compile the fragment shader
    shader frag_shader(::shader_type::fragment, fragment_shader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert_shader.id());
    glAttachShader(shaderProgram, frag_shader.id());
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint pos_attribute = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(pos_attribute);
    glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    GLint col_attribute = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(col_attribute);
    glVertexAttribPointer(col_attribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

    GLint tex_attribute = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(tex_attribute);
    glVertexAttribPointer(tex_attribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

    // Load textures
    glGenTextures(1, textures.data());

    resize_ir(width, height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ir.width(), ir.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.data());
    glUniform1i(glGetUniformLocation(shaderProgram, "tex_sampler"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void gl_image::resize_ir(int const &width, int const &height)
{
    ir = image_representation<std::uint8_t>(window_width, window_height, 4);
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
}

void gl_image::loop()
{
    if(focus)
    {
        /* Update texture */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ir.data());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

bool gl_image::is_on_focus()
{
    return focus;
}

int gl_image::gl_window_width()
{
    return window_width;
}

int gl_image::gl_window_height()
{
    return window_height;
}