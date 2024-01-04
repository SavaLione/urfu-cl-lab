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
 * @brief Application settings
 * @author Savelii Pototskii (savalione.com)
 * @date 15 Nov 2022
 */
#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include <cstddef>

class settings
{
public:
    /* Class */
    static settings &instance()
    {
        static settings s;
        return s;
    }

    /* Variables */
    bool get_gpu();
    bool get_cpu();
    std::size_t get_vector_size();
    std::size_t get_iteration_count();
    std::size_t get_laboratory_work();
    bool get_verbose();
    bool get_exit();

    void set_gpu(bool const &gpu);
    void set_cpu(bool const &cpu);
    void set_vector_size(std::size_t const &vector_size);
    void set_iteration_count(std::size_t const &iteration_count);
    void set_laboratory_work(std::size_t const &laboratory_work);
    void set_verbose(bool const &verbose);
    void set_exit(bool const &exit);

private:
    /* Class */
    settings();
    settings(settings const &)            = delete;
    settings &operator=(settings const &) = delete;

    /* Variables */
    bool gpu                    = false;
    bool cpu                    = false;
    std::size_t vector_size     = 102400000;
    std::size_t iteration_count = 100;
    std::size_t laboratory_work = 0;
    bool verbose                = false;
    bool exit                   = false;
};

#endif // CORE_SETTINGS_H