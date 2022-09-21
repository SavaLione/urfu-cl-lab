##=============================================================================
##
##  Copyright (c) Saveliy Pototskiy.
##  All rights reserved.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##
##  Copyright 2022 Saveliy Pototskiy (savalione.com).
##
##=============================================================================
# Try to find spdlog library and include dir.
#
# SPDLOG_FOUND
# SPDLOG_INCLUDE_DIR
# SPDLOG_LIBRARY
#

include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path( SPDLOG_INCLUDE_DIR
        NAMES
            spdlog
        PATHS
            ${PROJECT_SOURCE_DIR}/shared_external/spdlog/include
            ${PROJECT_SOURCE_DIR}/../shared_external/spdlog/include
            ${SPDLOG_LOCATION}/include
            $ENV{SPDLOG_LOCATION}/include
            $ENV{PROGRAMFILES}/SPDLOG/include
            ${SPDLOG_LOCATION}
            $ENV{SPDLOG_LOCATION}
            $ENV{Path}
            DOC "The directory where spdlog resides" )
    if(ARCH STREQUAL "x86")
      find_library( SPDLOG_LIBRARY
          NAMES
              spdlog
          PATHS
              ${SPDLOG_LOCATION}/lib
              $ENV{SPDLOG_LOCATION}/lib
              $ENV{PROGRAMFILES}/SPDLOG/lib
              $ENV{Path}
              DOC "The spdlog library")
    else()
      find_library( SPDLOG_LIBRARY
          NAMES
              spdlog
          PATHS
              ${SPDLOG_LOCATION}/lib
              $ENV{SPDLOG_LOCATION}/lib
              $ENV{PROGRAMFILES}/SPDLOG/lib
              $ENV{Path}
              DOC "The spdlog library")
    endif()
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( SPDLOG_INCLUDE_DIR
        NAMES
            spdlog
        PATHS
            ${SPDLOG_LOCATION}/include
            $ENV{SPDLOG_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where spdlog resides"
    )
    find_library( SPDLOG_LIBRARY
        NAMES
        spdlog
        PATHS
            ${SPDLOG_LOCATION}/lib
            $ENV{SPDLOG_LOCATION}/lib
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            /usr/lib/x86_64-linux-gnu
            NO_DEFAULT_PATH
            DOC "The spdlog library")
endif ()

find_package_handle_standard_args(spdlog DEFAULT_MSG
    SPDLOG_INCLUDE_DIR
    SPDLOG_LIBRARY
)

mark_as_advanced( SPDLOG_FOUND )
