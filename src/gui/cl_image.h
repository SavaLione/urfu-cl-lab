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
 * @brief Draw OpenCL image
 * @author Savelii Pototskii (savalione.com)
 * @date 22 Nov 2022
 */
#ifndef GUI_CL_IMAGE_H
#define GUI_CL_IMAGE_H

#include "gui/gl_image.h"

/*
    TODO: Delete this
    DO NOT USE THIS
*/
class [[deprecated]] cl_image : public gl_image
{
private:
    /* SDL2 */
    void loop() override;
    void init() override;
    void resize_ir(int const &width, int const &height) override;
};

#endif // CORE_CL_IMAGE_H