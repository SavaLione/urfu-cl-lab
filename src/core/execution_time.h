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
 * @brief Execution time
 * @author Saveliy Pototskiy (SavaLione)
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
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _stop;
};

#endif // CORE_EXECUTION_TIME_H