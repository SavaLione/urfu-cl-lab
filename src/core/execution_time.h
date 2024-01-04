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
 * @brief Execution time
 * @author Savelii Pototskii (savalione.com)
 * @date 21 Sep 2022
 */
#ifndef CORE_EXECUTION_TIME_H
#define CORE_EXECUTION_TIME_H

#include <chrono>

class execution_time
{
public:
    void start();
    void stop();

    long long const count_nanoseconds() const &
    {
        return (std::chrono::duration_cast<std::chrono::nanoseconds>(_stop - _start)).count();
    }

    long long const count_microseconds() const &
    {
        return (std::chrono::duration_cast<std::chrono::microseconds>(_stop - _start)).count();
    }

    long long const count_milliseconds() const &
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(_stop - _start)).count();
    }

    long long const count_seconds() const &
    {
        return (std::chrono::duration_cast<std::chrono::seconds>(_stop - _start)).count();
    }

    long long const count_minutes() const &
    {
        return (std::chrono::duration_cast<std::chrono::minutes>(_stop - _start)).count();
    }

    long long const count_hours() const &
    {
        return (std::chrono::duration_cast<std::chrono::hours>(_stop - _start)).count();
    }

private:
    std::chrono::system_clock::time_point _start;
    std::chrono::system_clock::time_point _stop;
};

#endif // CORE_EXECUTION_TIME_H