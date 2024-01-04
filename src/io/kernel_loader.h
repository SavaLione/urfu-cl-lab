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
 * @brief OpenCL kernel loader
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Sep 2022
 */
#ifndef IO_KERNEL_LOADER_H
#define IO_KERNEL_LOADER_H

#include <string>
#include <vector>

class kernel_loader
{
public:
    static kernel_loader &instance()
    {
        static kernel_loader kl;
        return kl;
    }

    void load();
    void load(std::string const &name);

    std::vector<std::string> const &get() const
    {
        return _string_kernels;
    }

    void print();

    void reset();
    void reload();

private:
    kernel_loader();
    kernel_loader(kernel_loader const &)            = delete;
    kernel_loader &operator=(kernel_loader const &) = delete;

    std::vector<std::string> _loaded_kernels;
    std::vector<std::string> _string_kernels;
};

#endif // IO_KERNEL_LOADER_H