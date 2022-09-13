#################################################################################
# Copyright (c) 2022, Savely Pototsky (SavaLione)
# All rights reserved.
#################################################################################
# Checking the minimum version
cmake_minimum_required (VERSION 3.13.1)

# Project Nyx
set(NYX_PROJECT "nyx")
set(NYX_PROJECT_VERSION "0.0.1")

project(${NYX_PROJECT}
    VERSION ${NYX_PROJECT_VERSION}
    LANGUAGES CXX
)

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")               # default
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")           # debug

#################################################################################
#                               Macros and functions                            #
#################################################################################
# Output directory
function(function_output_directory arg_project)
    set_target_properties(${arg_project}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # lib
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # lib
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction(function_output_directory)

#################################################################################
#                                    Libraries                                  #
#################################################################################
# OpenCL
find_package(OpenCL REQUIRED)

#################################################################################
#                                     Sources                                   #
#################################################################################
# core
set(NYX_CORE_SRC
    src/core/nyx.h
    src/core/nyx.cpp
)

# OpenCL kernels
set(NYX_CL_KERNELS_SRC
    src/cl/kernels/test.cl
)

# cl
set(NYX_CL_SRC
    ${NYX_CL_KERNELS_SRC}
)

# Nyx application
set(NYX_SRC
    ${NYX_CORE_SRC}
    ${NYX_CL_SRC}
)

#################################################################################
#                                     Executable                                #
#################################################################################
add_executable(${NYX_PROJECT} ${NYX_SRC})
target_include_directories(${NYX_PROJECT} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# C++ features
target_compile_features(${NYX_PROJECT} PRIVATE cxx_auto_type)

#################################################################################
#                                Libraries include                              #
#################################################################################
# OpenCL
target_link_libraries(${NYX_PROJECT} OpenCL::OpenCL)
target_include_directories(${NYX_PROJECT} PUBLIC ${OPENCL_INCLUDE_DIR})

#################################################################################
#                                 Output directory                              #
#################################################################################

function_output_directory(${NYX_PROJECT})   # Nyx application