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
 * @brief OpenGL program representation
 * @author Saveliy Pototskiy (SavaLione)
 * @date 16 Dec 2022
 */
#include "gui/program.h"

program::~program()
{
    if(_id)
        glDeleteProgram(_id);
}

program::program(shader const &vertex, shader const &fragment)
{
    GLuint program = glCreateProgram();
    // GLuint vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    // GLuint fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vertex.id());
    glAttachShader(program, fragment.id());
    glLinkProgram(program);
    glValidateProgram(program);

    // glDeleteShader(vs);
    // glDeleteShader(fs);

    _id = program;
}

program::program(std::string const &vertex, std::string const &fragment)
{
    GLuint program = glCreateProgram();
    // GLuint vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    // GLuint fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
    shader vertex_shader(shader_type::vertex, vertex);
    shader fragment_shader(shader_type::fragment, fragment);

    glAttachShader(program, vertex_shader.id());
    glAttachShader(program, fragment_shader.id());
    glLinkProgram(program);
    glValidateProgram(program);

    // glDeleteShader(vs);
    // glDeleteShader(fs);

    _id = program;
}