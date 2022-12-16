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
 * @brief Image representation
 * @author Saveliy Pototskiy (SavaLione)
 * @date 22 Nov 2022
 */
#ifndef GUI_IMAGE_REPRESENTATION_H
#define GUI_IMAGE_REPRESENTATION_H

#include <cstdint>
#include <cstdlib>
#include <vector>

class image_representation
{
public:
    struct vec3
    {
        std::int8_t r;
        std::int8_t g;
        std::int8_t b;
    };

    struct vec4
    {
        std::int8_t r;
        std::int8_t g;
        std::int8_t b;
        std::int8_t a;
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
    uint8_t *data();
    uint8_t const *const_data() const;
    void fill_zeros();
    void set_pixel(std::size_t x, std::size_t y, vec4 color);

private:
    std::size_t _width;
    std::size_t _height;
    std::size_t _depth;
    std::vector<uint8_t> _image;
};

#endif // CORE_IMAGE_REPRESENTATION_H