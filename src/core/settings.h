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
 * @brief Application settings
 * @author Saveliy Pototskiy (SavaLione)
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
    bool gpu                    = true;
    bool cpu                    = true;
    std::size_t vector_size     = 102400000;
    std::size_t iteration_count = 100;
    std::size_t laboratory_work = 1;
    bool verbose                = false;
    bool exit                   = false;
};

#endif // CORE_SETTINGS_H