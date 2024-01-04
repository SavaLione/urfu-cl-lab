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
 * @brief OpenGL program representation
 * @author Savelii Pototskii (savalione.com)
 * @date 16 Dec 2022
 */
#include "gui/program.h"

program::~program()
{
    if(_id)
        glDeleteProgram(_id);
}

GLuint _program::_create(std::string const &vertex, std::string const &fragment)
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

    return program;
}