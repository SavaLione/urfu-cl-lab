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
 * @date 16 Dec 2022
 */
#ifndef GUI_SHADER_H
#define GUI_SHADER_H

// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include <cstddef>
#include <string>
#include <type_traits>

enum shader_type
{
    fragment = 0x8B30,
    vertex   = 0x8B31
};

namespace _shader
{
    /* Create program */
    GLuint _create(shader_type const &type, std::string const &source);
} // namespace _shader

class shader
{
public:
    shader(shader_type const &type, std::string const &source) : _type(type), _source(source), _id(_shader::_create(type, source)) {}

    ~shader();

    /* Get shader id */
    GLuint const &id() const
    {
        return _id;
    }

    shader(shader const &s) : _type(s._type), _source(s._source), _id(_shader::_create(s._type, s._source)) {}

    shader &operator=(shader const &s)
    {
        _type   = s._type;
        _source = s._source;
        _id     = _shader::_create(_type, _source);
        return *this;
    }

    void swap(shader &s)
    {
        std::swap(this->_type, s._type);
        std::swap(this->_source, s._source);
        std::swap(this->_id, s._id);
    }

private:
    /* Shader id */
    GLuint _id = 0;

    /* Shader type */
    shader_type _type;

    /* Shader source code */
    std::string _source;
};

#endif // GUI_SHADER_H