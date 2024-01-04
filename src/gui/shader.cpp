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
 * @brief OpenGL shader representation
 * @author Savelii Pototskii (savalione.com)
 * @date 16 Nov 2022
 */
#include "gui/shader.h"

#include <stdexcept>
#include <vector>

shader::~shader()
{
    if(_id)
        glDeleteShader(_id);
}

GLuint _shader::_create(shader_type const &type, std::string const &source)
{
    GLuint ret_id   = glCreateShader(type);
    char const *src = source.c_str();

    glShaderSource(ret_id, 1, &src, nullptr);
    glCompileShader(ret_id);

    int result;

    glGetShaderiv(ret_id, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(ret_id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length);
        glGetShaderInfoLog(ret_id, length, &length, message.data());
        std::string _err = "Shader compilation error: ";
        for(size_t i = 0; i < message.size(); i++)
            _err += message[i];
        glDeleteShader(ret_id);
        throw std::runtime_error(_err);
    }

    return ret_id;
}