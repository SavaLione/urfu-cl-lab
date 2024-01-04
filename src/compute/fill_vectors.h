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
 * @brief Fill vectors with test data
 * @author Savelii Pototskii (savalione.com)
 * @date 26 Oct 2022
 */
#ifndef COMPUTE_FILL_VECTORS_H
#define COMPUTE_FILL_VECTORS_H

#include <iterator>
#include <vector>

template<typename iterator_type>
void fill_vectors(iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_b, iterator_type end_iterator_b)
{
    typedef typename std::iterator_traits<iterator_type>::value_type data_type;
    typedef typename std::iterator_traits<iterator_type> data_size;

    std::size_t size_of_a = sizeof(data_type) * (end_iterator_a - start_iterator_a);
    std::size_t size_of_b = sizeof(data_type) * (end_iterator_b - start_iterator_b);

    std::size_t size_a = sizeof(data_size) * (end_iterator_a - start_iterator_a);
    std::size_t size_b = sizeof(data_size) * (end_iterator_b - start_iterator_b);

    if(size_of_a != size_of_b)
    {
        throw std::logic_error("Iterators are not equal.");
    }

#pragma omp parallel for
    for(std::size_t i = 0; i < size_a; i++)
    {
        start_iterator_a[i] = (float)i / 2;
    }

#pragma omp parallel for
    for(std::size_t i = 0; i < size_b; i++)
    {
        start_iterator_b[i] = (float)i * 2;
    }
}

#endif // COMPUTE_FILL_VECTORS_H