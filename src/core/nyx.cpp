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
 * @brief Nyx application
 * @author Saveliy Pototskiy (SavaLione)
 * @date 21 Sep 2022
 */
#include "core/nyx.h"

#include "compute/compute.h"
#include "compute/cpu.h"
#include "compute/kernel_loader.h"
#include "compute/opencl.h"

int main()
{
	/* Kernel loader instance */
	kernel_loader &kernel_loader_instance = kernel_loader::instance();
	kernel_loader_instance.load();

	compute c;

	c.print_info();

	c.compute_vec_16("addition_vector_16");
	c.compute_vec_8("addition_vector_8");
	c.compute_vec_4("addition_vector_4");
	c.compute_vec_2("addition_vector_2");

	c.compute_vec_16("divide_vector_16");
	c.compute_vec_8("divide_vector_8");
	c.compute_vec_4("divide_vector_4");
	c.compute_vec_2("divide_vector_2");

	c.compute_vec_16("exponentiation_vector_16");
	c.compute_vec_8("exponentiation_vector_8");
	c.compute_vec_4("exponentiation_vector_4");
	c.compute_vec_2("exponentiation_vector_2");

	c.compute_vec_16("multiple_vector_16");
	c.compute_vec_8("multiple_vector_8");
	c.compute_vec_4("multiple_vector_4");
	c.compute_vec_2("multiple_vector_2");

	c.compute_vec_16("remove_vector_16");
	c.compute_vec_8("remove_vector_8");
	c.compute_vec_4("remove_vector_4");
	c.compute_vec_2("remove_vector_2");

	c.compute_one_vec_16("log_vector_16");
	c.compute_one_vec_16("log_vector_8");
	c.compute_one_vec_16("log_vector_4");
	c.compute_one_vec_16("log_vector_2");
}
