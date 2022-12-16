/*
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright (c) 2021-2022, Saveliy Pototskiy (SavaLione) (savalione.com)
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
 * @brief A simple function to determine the compiler name and version
 * @author Saveliy Pototskiy (SavaLione)
 * @date 3 Mar 2021
 */
#include "platform/compiler_version.h"

std::string compiler_version()
{
    /*
        clang
    */
    #if defined __clang__
        #if __clang_major__ == 1
            return "Clang 1.x";
        #elif __clang_major__ == 2
            return "Clang 2.x";
        #elif __clang_major__ >= 3
            return "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
        #else
            return "Clang unknown version";
        #endif
    #endif // __clang__

    /*
        Microsoft Visual c++
    */
    #if defined _MSC_VER
        #if _MSC_VER == 800
            return "Microsoft Visual C++ 1.0";
        #elif _MSC_VER == 900
            return "Microsoft Visual C++ 3.0";
        #elif _MSC_VER == 1000
            return "Microsoft Visual C++ 4.0";
        #elif _MSC_VER == 1020
            return "Microsoft Visual C++ 4.2";
        #elif _MSC_VER == 1100
            return "Microsoft Visual C++ 5.0";
        #elif _MSC_VER == 1200
            return "Microsoft Visual C++ 6.0";
        #elif _MSC_VER == 1300
            return "Microsoft Visual C++ 7.0 (Microsoft Visual Studio 2002)";
        #elif _MSC_VER == 1310
            return "Microsoft Visual C++ 7.1 (Microsoft Visual Studio 2003)";
        #elif _MSC_VER == 1400
            return "Microsoft Visual C++ 8.0 (Microsoft Visual Studio 2005)";
        #elif _MSC_VER == 1500
            #ifdef _MSC_FULL_VER == 150021022
                return "Microsoft Visual C++ 9.0 (Microsoft Visual Studio 2008)";
            #elif _MSC_FULL_VER == 150030729
                return "Microsoft Visual C++ 9.0 SP 1 (Microsoft Visual Studio 2008 SP 1)";
            #else
                return "Microsoft Visual C++ 9.0 SP ? (Microsoft Visual Studio 2008 SP ?)";
            #endif
        #elif _MSC_VER == 1600
            #if _MSC_FULL_VER == 160030319
                return "Microsoft Visual C++ 10.0 (Microsoft Visual Studio 2010)";
            #elif _MSC_FULL_VER == 160040219
                return "Microsoft Visual C++ 10.0 SP 1(Microsoft Visual Studio 2010 SP 1)";
            #else
                return "Microsoft Visual C++ 10.0 SP ? (Microsoft Visual Studio 2010 SP ?)";
            #endif
        #elif _MSC_VER == 1700
            return "Microsoft Visual C++ 11.0 (Microsoft Visual Studio 2012)";
        #elif _MSC_VER == 1800
            return "Microsoft Visual C++ 12.0 (Microsoft Visual Studio 2013)";
        #elif _MSC_VER == 1900
            #if _MSC_FULL_VER == 190023026
                return "Microsoft Visual C++ 14.0 (Microsoft Visual Studio 2015)";
            #elif _MSC_FULL_VER == 190023506
                return "Microsoft Visual C++ 14.0 (Microsoft Visual Studio 2015 Update 1)";
            #elif _MSC_FULL_VER == 190023918
                return "Microsoft Visual C++ 14.0 (Microsoft Visual Studio 2015 Update 2)";
            #elif _MSC_FULL_VER == 190024210
                return "Microsoft Visual C++ 14.0 (Microsoft Visual Studio 2015 Update 3)";
            #else
                return "Microsoft Visual C++ 14.0 (Microsoft Visual Studio 2015 Update ?)";
            #endif
        #elif _MSC_VER == 1910
            return "Microsoft Visual C++ 15.0 (Microsoft Visual Studio 2017)";
        #else
            return "Microsoft Visual C++ unknown version";
        #endif
    #endif // _MSC_VER

    /*
        gnuc
    */
    #if defined __GNUC__
        #if defined __GNUC_PATCHLEVEL__
            return "GCC " + std::to_string(__GNUC__) + "." +  std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
        #else
            return "GCC " + std::to_string(__GNUC__) + "." +  std::to_string(__GNUC_MINOR__);
        #endif
    #endif // __GNUC__
    
    /*
        unknown
    */
   return "unknown";
}