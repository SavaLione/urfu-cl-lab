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
# Better example of cmake find module is here: https://github.com/Kitware/CMake/blob/master/Modules/FindGLEW.cmake

# Check whether to search static or dynamic libs
# set( CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES} )

# if( ${SDL2_USE_STATIC_LIBS} )
#     set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )
# else()
#     set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV} )
# endif()

# SDL2 main library
if (WIN32)
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_main.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
        DOC "The directory where main sdl2 include directory resides"
    )

    find_library( SDL2_MAIN_LIB
        NAMES
            SDL2main
            libSDL2main
        PATHS
            $ENV{Path}
            $ENV{Path}/lib
        DOC "The directory where main sdl2 library resides"
    )
elseif(${CMAKE_HOST_UNIX})
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_main.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
            ${SDL2_LOCATION}/include
            $ENV{SDL2_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
        DOC "The directory where main sdl2 include directory resides"
    )

    if(ARCH STREQUAL "x86")
        find_library(SDL2_MAIN_LIB
            NAMES
                SDL2main
                libSDL2main
            PATHS
                $ENV{Path}
                ${SDL2_LOCATION}/lib
                $ENV{SDL2_LOCATION}/lib
                /usr/lib
                /usr/local/lib
                /sw/lib
                /opt/local/lib
                NO_DEFAULT_PATH
            DOC "The directory where main sdl2 library (x86) resides"
        )
    else()
        find_library(SDL2_MAIN_LIB
        NAMES
            SDL2main
            libSDL2main
        PATHS
            $ENV{Path}/lib64
            ${SDL2_LOCATION}/lib64
            $ENV{SDL2_LOCATION}/lib64
            /usr/lib64
            /usr/local/lib64
            /usr/lib/x86_64-linux-gnu 
            $ENV{Path}
            NO_DEFAULT_PATH
        DOC "The directory where main sdl2 library (x64) resides"
        )
    endif()
endif()

if (SDL2_MAIN_LIB)
    set(SDL2_MAIN_LIB_FOUND TRUE)
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_MAIN_LIB})
    add_library(SDL2::main INTERFACE IMPORTED)
    set_target_properties(SDL2::main
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_MAIN_LIB}"
    )
else()
    set(SDL2_MAIN_LIB_FOUND FALSE)
endif()

# SDL2 core library
if (WIN32)
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
        DOC "The directory where core sdl2 include directory resides"
    )

    find_library( SDL2_CORE_LIB
        NAMES
            SDL2
            libSDL2
        PATHS
            $ENV{Path}
            $ENV{Path}/lib
        DOC "The directory where core sdl2 library resides"
    )
elseif(${CMAKE_HOST_UNIX})
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
            ${SDL2_LOCATION}/include
            $ENV{SDL2_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
        DOC "The directory where core sdl2 include directory resides"
    )

    if(ARCH STREQUAL "x86")
        find_library(SDL2_CORE_LIB
            NAMES
                SDL2
                libSDL2
            PATHS
                $ENV{Path}
                ${SDL2_LOCATION}/lib
                $ENV{SDL2_LOCATION}/lib
                /usr/lib
                /usr/local/lib
                /sw/lib
                /opt/local/lib
                NO_DEFAULT_PATH
            DOC "The directory where core sdl2 library (x86) resides"
        )
    else()
        find_library(SDL2_CORE_LIB
        NAMES
            SDL2
            libSDL2
        PATHS
            $ENV{Path}/lib64
            ${SDL2_LOCATION}/lib64
            $ENV{SDL2_LOCATION}/lib64
            /usr/lib64
            /usr/local/lib64
            /usr/lib/x86_64-linux-gnu 
            $ENV{Path}
            NO_DEFAULT_PATH
        DOC "The directory where core sdl2 library (x64) resides"
        )
    endif()
endif()

if (SDL2_CORE_LIB)
    set(SDL2_CORE_LIB_FOUND TRUE)
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_CORE_LIB})
    add_library(SDL2::core INTERFACE IMPORTED)
    set_target_properties(SDL2::core
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_CORE_LIB}"
    )
else()
    set(SDL2_CORE_LIB_FOUND FALSE)
endif()

# SDL2 mixer library
if (WIN32)
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_mixer.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
        DOC "The directory where mixer sdl2 include directory resides"
    )

    find_library( SDL2_MIXER_LIB
        NAMES
            SDL2_mixer
            libSDL2_mixer
        PATHS
            $ENV{Path}
            $ENV{Path}/lib
        DOC "The directory where mixer sdl2 library resides"
    )
elseif(${CMAKE_HOST_UNIX})
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_mixer.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
            ${SDL2_LOCATION}/include
            $ENV{SDL2_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
        DOC "The directory where mixer sdl2 include directory resides"
    )

    if(ARCH STREQUAL "x86")
        find_library(SDL2_MIXER_LIB
            NAMES
                SDL2
                libSDL2
            PATHS
                $ENV{Path}
                ${SDL2_LOCATION}/lib
                $ENV{SDL2_LOCATION}/lib
                /usr/lib
                /usr/local/lib
                /sw/lib
                /opt/local/lib
                NO_DEFAULT_PATH
            DOC "The directory where mixer sdl2 library (x86) resides"
        )
    else()
        find_library(SDL2_MIXER_LIB
            NAMES
                SDL2_mixer
                libSDL2_mixer
            PATHS
                $ENV{Path}/lib64
                ${SDL2_LOCATION}/lib64
                $ENV{SDL2_LOCATION}/lib64
                /usr/lib64
                /usr/local/lib64
                /usr/lib/x86_64-linux-gnu 
                $ENV{Path}
                NO_DEFAULT_PATH
            DOC "The directory where mixer sdl2 library (x64) resides"
        )
    endif()
endif()

if (SDL2_MIXER_LIB)
    set(SDL2_MIXER_LIB_FOUND TRUE)
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_MIXER_LIB})
    add_library(SDL2::mixer INTERFACE IMPORTED)
    set_target_properties(SDL2::mixer
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_MIXER_LIB}"
    )
else()
    set(SDL2_MIXER_LIB_FOUND FALSE)
endif()

# SDL2 test library
if (WIN32)
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_test.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
        DOC "The directory where test sdl2 include directory resides"
    )

    find_library( SDL2_TEST_LIB
        NAMES
            SDL2_test
            libSDL2_test
        PATHS
            $ENV{Path}
            $ENV{Path}/lib
        DOC "The directory where test sdl2 library resides"
    )
elseif(${CMAKE_HOST_UNIX})
    find_path( SDL2_INCLUDE_DIRS
        NAMES
            SDL2/SDL_test.h
        PATHS
            $ENV{Path}
            $ENV{Path}/include
            ${SDL2_LOCATION}/include
            $ENV{SDL2_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
        DOC "The directory where test sdl2 include directory resides"
    )

    if(ARCH STREQUAL "x86")
        find_library(SDL2_TEST_LIB
            NAMES
                SDL2
                libSDL2
            PATHS
                $ENV{Path}
                ${SDL2_LOCATION}/lib
                $ENV{SDL2_LOCATION}/lib
                /usr/lib
                /usr/local/lib
                /sw/lib
                /opt/local/lib
                NO_DEFAULT_PATH
            DOC "The directory where test sdl2 library (x86) resides"
        )
    else()
        find_library(SDL2_TEST_LIB
            NAMES
                SDL2_test
                libSDL2_test
            PATHS
                $ENV{Path}/lib64
                ${SDL2_LOCATION}/lib64
                $ENV{SDL2_LOCATION}/lib64
                /usr/lib64
                /usr/local/lib64
                /usr/lib/x86_64-linux-gnu 
                $ENV{Path}
                NO_DEFAULT_PATH
            DOC "The directory where test sdl2 library (x64) resides"
        )
    endif()
endif()

if (SDL2_TEST_LIB)
    set(SDL2_TEST_LIB_FOUND TRUE)
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_TEST_LIB})
    add_library(SDL2::test INTERFACE IMPORTED)
    set_target_properties(SDL2::test
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_TEST_LIB}"
    )
else()
    set(SDL2_TEST_LIB_FOUND FALSE)
endif()



# set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV} )

include(FindPackageHandleStandardArgs)
# include(SelectLibraryConfigurations)

find_package_handle_standard_args(SDL2
    REQUIRED_VARS SDL2_INCLUDE_DIRS
    HANDLE_COMPONENTS
)

mark_as_advanced(
    SDL2_INCLUDE_DIRS
    SDL2_LIBRARIES
    SDL2_MAIN_LIB
    SDL2_CORE_LIB
    SDL2_MIXER_LIB
    SDL2_TEST_LIB
)
