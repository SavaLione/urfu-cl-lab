#################################################################################
# Copyright (c) 2022, Savely Pototsky (SavaLione)
# All rights reserved.
#################################################################################
# Checking the minimum version
cmake_minimum_required (VERSION 3.13.1)

# Add module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Project Nyx
set(NYX_PROJECT "nyx")
set(NYX_PROJECT_VERSION "0.0.1")

project(${NYX_PROJECT}
    VERSION ${NYX_PROJECT_VERSION}
    LANGUAGES CXX
)

# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -DNDEBUG -fopenmp")
else()
    message(STATUS "Unknown build type: " ${CMAKE_BUILD_TYPE})
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Export commands
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

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

# OpenMP
find_package(OpenMP)

# spdlog
find_package(spdlog REQUIRED)

#################################################################################
#                                     Sources                                   #
#################################################################################
# core
set(NYX_CORE_SRC
    src/core/nyx.h
    src/core/nyx.cpp
    src/core/execution_time.h
    src/core/execution_time.cpp
)

# OpenCL kernels
set(NYX_COMPUTE_KERNELS_SRC
    src/compute/kernels/simple_add.cl
    src/compute/kernels/simple_divide.cl
    src/compute/kernels/simple_exponentiation.cl
    src/compute/kernels/simple_multiple.cl
    src/compute/kernels/simple_remove.cl
    src/compute/kernels/simple_vec_add.cl
    src/compute/kernels/addition_vector_2.cl
    src/compute/kernels/addition_vector_4.cl
    src/compute/kernels/addition_vector_8.cl
    src/compute/kernels/addition_vector_16.cl
    src/compute/kernels/divide_vector_2.cl
    src/compute/kernels/divide_vector_4.cl
    src/compute/kernels/divide_vector_8.cl
    src/compute/kernels/divide_vector_16.cl
    src/compute/kernels/exponentiation_vector_2.cl
    src/compute/kernels/exponentiation_vector_4.cl
    src/compute/kernels/exponentiation_vector_8.cl
    src/compute/kernels/exponentiation_vector_16.cl
    src/compute/kernels/log_vector_2.cl
    src/compute/kernels/log_vector_4.cl
    src/compute/kernels/log_vector_8.cl
    src/compute/kernels/log_vector_16.cl
    src/compute/kernels/multiple_vector_2.cl
    src/compute/kernels/multiple_vector_4.cl
    src/compute/kernels/multiple_vector_8.cl
    src/compute/kernels/multiple_vector_16.cl
    src/compute/kernels/remove_vector_2.cl
    src/compute/kernels/remove_vector_4.cl
    src/compute/kernels/remove_vector_8.cl
    src/compute/kernels/remove_vector_16.cl
)

# OpenCL and CPU source files
set(NYX_COMPUTE_SRC
    src/compute/compute.h
    src/compute/compute.cpp
    src/compute/cpu.h
    src/compute/cpu.cpp
    src/compute/kernel_loader.h
    src/compute/kernel_loader.cpp
    src/compute/opencl.h
    src/compute/opencl.cpp
    ${NYX_COMPUTE_KERNELS_SRC}
)

# Nyx application
set(NYX_SRC
    ${NYX_CORE_SRC}
    ${NYX_COMPUTE_SRC}
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

# OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(${NYX_PROJECT} OpenMP::OpenMP_CXX)
endif()

# spdlog
target_link_libraries(${NYX_PROJECT} ${SPDLOG_LIBRARY})
target_include_directories(${NYX_PROJECT} PUBLIC ${SPDLOG_INCLUDE_DIR})

#################################################################################
#                                 Output directory                              #
#################################################################################
function_output_directory(${NYX_PROJECT})                       # Nyx application
file(COPY src/compute/kernels DESTINATION ${CMAKE_BINARY_DIR}/bin/)  # OpenCL kernels destination directory