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
 * @brief Image representation
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Nov 2022
 */
#ifndef GUI_IMAGE_REPRESENTATION_H
#define GUI_IMAGE_REPRESENTATION_H

#include <cstdint>
#include <cstdlib>
#include <vector>

template<typename T>
class image_representation
{
public:
    struct vec3
    {
        T r;
        T g;
        T b;
    };

    struct vec4
    {
        T r;
        T g;
        T b;
        T a;
    };

    image_representation() : _width(0), _height(0), _depth(0)
    {
        _image.clear();
    }

    image_representation(std::size_t const &width, std::size_t const &height, std::size_t const &depth) : _width(width), _height(height), _depth(depth)
    {
        std::size_t size = width * height * depth;
        _image.resize(size, 0);
    }

    std::size_t const &width() const;
    std::size_t const &height() const;
    std::size_t const &depth() const;
    std::size_t size();
    T *data();
    T const *const_data() const;
    void fill_zeros();
    void set_pixel(std::size_t x, std::size_t y, vec4 color);
    void clear();

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _depth;
    std::vector<T> _image;
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
std::size_t const &image_representation<T>::width() const
{
    return _width;
}

template<typename T>
std::size_t const &image_representation<T>::height() const
{
    return _height;
}

template<typename T>
std::size_t const &image_representation<T>::depth() const
{
    return _depth;
}

template<typename T>
std::size_t image_representation<T>::size()
{
    return _image.size();
}

template<typename T>
T *image_representation<T>::data()
{
    return _image.data();
}

template<typename T>
T const *image_representation<T>::const_data() const
{
    return _image.data();
}

template<typename T>
void image_representation<T>::fill_zeros()
{
    for(std::size_t i = 0; i < _image.size(); i++)
        _image[i] = 0;
}

template<typename T>
void image_representation<T>::set_pixel(std::size_t x, std::size_t y, vec4 color)
{
    _image[(y * _width + x) * 4 + 0] = color.r;
    _image[(y * _width + x) * 4 + 1] = color.g;
    _image[(y * _width + x) * 4 + 2] = color.b;
    _image[(y * _width + x) * 4 + 3] = color.a;
}

template<typename T>
void image_representation<T>::clear()
{
    _width  = 0;
    _height = 0;
    _depth  = 0;
    _image.clear();
}

#endif // CORE_IMAGE_REPRESENTATION_H