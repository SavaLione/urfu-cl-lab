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
 * @brief New gpu functions and etc.
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Nov 2022
 */
#ifndef COMPUTE_NEW_GPU_H
#define COMPUTE_NEW_GPU_H

#include <boost/compute/types.hpp>

#include "gui/image_representation.h"
#include "gui/buffer_representation.h"

void draw_image_cl(image_representation<std::uint8_t> &img);
void draw_image_cl(image_representation<std::uint8_t> &img, std::string const &kern);
void draw_write_only_cl(image_representation<std::uint8_t> &img, std::string const &kern);
void draw_image_buffer_cl(image_representation<std::uint8_t> &img, buffer_representation<boost::compute::uchar4_> &buff);

#endif // COMPUTE_NEW_GPU_H