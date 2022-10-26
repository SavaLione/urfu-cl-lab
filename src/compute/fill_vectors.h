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
 * @brief Fill vectors with test data
 * @author Saveliy Pototskiy (SavaLione)
 * @date 26 Oct 2022
 */
#ifndef COMPUTE_FILL_VECTORS_H
#define COMPUTE_FILL_VECTORS_H

#include <iterator>
#include <vector>

template<typename iterator_type>
void fill_vectors(
	iterator_type start_iterator_a, iterator_type end_iterator_a, iterator_type start_iterator_b, iterator_type end_iterator_b)
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