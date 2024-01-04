##=============================================================================
##  SPDX-License-Identifier: GPL-2.0-or-later
##
##  Copyright (C) 2023-2024 Savelii Pototskii (savalione.com)
##  All rights reserved.
##
##  Author: Savelii Pototskii <monologuesplus@gmail.com>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
## 
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
## 
##  You should have received a copy of the GNU General Public License
##  along with this program. If not, see <http://www.gnu.org/licenses/>.
##
##=============================================================================
# This file tries to find glm library.
#
# Better example of cmake find module is here: https://github.com/Kitware/CMake/blob/master/Modules/FindGLEW.cmake
# 
# GLM_FOUND
# GLM_INCLUDE_DIR
#

include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path( GLM_INCLUDE_DIR
        NAMES
        glm
        PATHS
            ${PROJECT_SOURCE_DIR}/shared_external/glm/include
            ${PROJECT_SOURCE_DIR}/../shared_external/glm/include
            ${GLM_LOCATION}/include
            $ENV{GLM_LOCATION}/include
            $ENV{PROGRAMFILES}/glm/include
            ${GLM_LOCATION}
            $ENV{GLM_LOCATION}
            $ENV{Path}
            DOC "The directory where glm resides" )
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( GLM_INCLUDE_DIR
        NAMES
        glm
        PATHS
            ${GLM_LOCATION}/include
            $ENV{GLM_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where glm resides"
    )
endif ()

find_package_handle_standard_args(glm DEFAULT_MSG
    GLM_INCLUDE_DIR
)

mark_as_advanced( GLM_FOUND )
