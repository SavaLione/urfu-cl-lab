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
 * @brief Buffer representation for OpenCL
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Nov 2022
 */
#ifndef GUI_BUFFER_REPRESENTATION_H
#define GUI_BUFFER_REPRESENTATION_H

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

#endif // GUI_BUFFER_REPRESENTATION_H