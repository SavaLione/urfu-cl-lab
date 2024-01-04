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
#ifndef GUI_PROGRAM_H
#define GUI_PROGRAM_H

#include "gui/shader.h"
#include <type_traits>

namespace _program
{
    GLuint _create(std::string const &vertex, std::string const &fragment);
} // namespace _program

class program
{
public:
    program() = default;

    /* Create program */
    program(std::string const &vertex, std::string const &fragment) : _vertex_source(vertex), _fragment_source(fragment), _id(_program::_create(vertex, fragment)) {}

    ~program();

    /* Get program id */
    GLuint const &id() const
    {
        return _id;
    }

    program(program const &p) : _vertex_source(p._vertex_source), _fragment_source(p._fragment_source), _id(_program::_create(p._vertex_source, p._fragment_source)) {}

    program &operator=(program const &p)
    {
        _vertex_source   = p._vertex_source;
        _fragment_source = p._fragment_source;
        _id              = _program::_create(p._vertex_source, p._fragment_source);
        return *this;
    }

    void swap(program &p)
    {
        std::swap(this->_vertex_source, p._vertex_source);
        std::swap(this->_fragment_source, p._fragment_source);
        std::swap(this->_id, p._id);
    }

private:
    /* Program id */
    GLuint _id = 0;

    /* Vertex shader source code */
    std::string _vertex_source;

    /* Fragment shader source code */
    std::string _fragment_source;
};

#endif // GUI_PROGRAM_H