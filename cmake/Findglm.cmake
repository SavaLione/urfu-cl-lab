##=============================================================================
##
##  Copyright (c) Saveliy Pototskiy.
##  All rights reserved.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##
##  Copyright 2023 Saveliy Pototskiy (savalione.com).
##
##=============================================================================
# Try to find glm include dir.
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
