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
 * @brief OpenGL vao triangle
 * @author Saveliy Pototskiy (SavaLione)
 * @date 27 Nov 2022
 */
#include "core/vao_triangle.h"

#include <cstddef>
#include <string>

#include "io/log/logger.h"

GLuint compile_shader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)malloc(length);
        glGetShaderInfoLog(id, length, &length, message);
        std::string _err = "Compile shader error: ";
        _err += message;
        throw std::runtime_error(_err);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

GLuint create_program(const std::string &vertex_shader, const std::string &fragment_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

std::string _fragment_shader = R"shader(
#version 330 core

// Ouput data
out vec3 color;

void main()
{
	// Output color = red 
	color = vec3(1,0,0);
}
)shader";

std::string _vertex_shader = R"shader(
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

void main()
{
    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
}
)shader";

vao_triangle::vao_triangle()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Create and compile program
    program_id = create_program(_vertex_shader, _fragment_shader);

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

vao_triangle::~vao_triangle()
{
    // Cleanup VBO
    if(vertex_buffer != 0)
        glDeleteBuffers(1, &vertex_buffer);
    if(vertex_array_id != 0)
        glDeleteVertexArrays(1, &vertex_array_id);
    if(program_id != 0)
        glDeleteProgram(program_id);
}

void vao_triangle::loop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(program_id);

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