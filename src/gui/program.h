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