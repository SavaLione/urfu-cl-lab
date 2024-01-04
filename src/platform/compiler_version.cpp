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
 * @brief A simple function to determine the compiler name and version
 * @author Savelii Pototskii (savalione.com)
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