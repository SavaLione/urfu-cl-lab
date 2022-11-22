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
 * @brief Buffer representation for OpenCL
 * @author Saveliy Pototskiy (SavaLione)
 * @date 22 Nov 2022
 */
#ifndef CORE_BUFFER_REPRESENTATION_H
#define CORE_BUFFER_REPRESENTATION_H

#include <cstdint>
#include <cstdlib>
#include <vector>

template<typename cl_type>
class buffer_representation
{
public:
    buffer_representation(std::size_t const &width, std::size_t const &height) : _width(width), _height(height)
    {
        std::size_t size = width * height;
        _buffer.resize(size);
    }

    std::size_t const &width() const;
    std::size_t const &height() const;
    std::size_t size();
    cl_type *data();
    cl_type const *const_data() const;
    void const fill(cl_type const &data) const;

private:
    std::size_t _width;
    std::size_t _height;
    std::vector<cl_type> _buffer;
};

///////////////////////////////////////////////////////////////////////////////

template<typename cl_type>
std::size_t const &buffer_representation<cl_type>::width() const
{
    return _width;
}

template<typename cl_type>
std::size_t const &buffer_representation<cl_type>::height() const
{
    return _height;
}

template<typename cl_type>
std::size_t buffer_representation<cl_type>::size()
{
    return _buffer.size();
}

template<typename cl_type>
cl_type *buffer_representation<cl_type>::data()
{
    return _buffer.data();
}

template<typename cl_type>
cl_type const *buffer_representation<cl_type>::const_data() const
{
    return _buffer.data();
}

template<typename cl_type>
void const buffer_representation<cl_type>::fill(cl_type const &data) const
{
    for(std::size_t i = 0; i < _buffer; i++)
        _buffer[i] = data;
}

#endif // CORE_BUFFER_REPRESENTATION_H