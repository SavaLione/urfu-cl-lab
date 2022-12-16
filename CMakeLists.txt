cmake_minimum_required (VERSION 3.13.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(nyx
    VERSION "2.1.0"
    DESCRIPTION "urfu OpenCL/OpenGL laboratory works. Saveliy Pototskiy (savalione.com) <monologuesplus@gmail.com>"
    HOMEPAGE_URL "https://github.com/SavaLione/urfu-cl-lab"
    LANGUAGES CXX
)

# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -DNDEBUG")
else()
    message(STATUS "Unknown build type: " ${CMAKE_BUILD_TYPE})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Output directory macro
function(function_output_directory arg_project)
    set_target_properties(${arg_project}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction(function_output_directory)

# Libraries
find_package(Threads)
find_package(OpenMP)
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED)
find_package(fmt)

if(fmt_FOUND)
    set(NYX_EXTERNAL_FMT ON)
else()
    set(NYX_EXTERNAL_FMT OFF)
endif()

# CMake configure
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.h)

# Sources
set(NYX_CORE_SRC
    src/core/buffer_representation.h
    src/core/buffer_representation.cpp
    src/core/cl_image.h
    src/core/cl_image.cpp
    src/core/cl_mandelbrot.h
    src/core/cl_mandelbrot.cpp
    src/core/execution_time.h
    src/core/execution_time.cpp
    src/core/gl_image.h
    src/core/gl_image.cpp
    src/core/image_representation.h
    src/core/image_representation.cpp
    src/core/new_gpu.h
    src/core/new_gpu.cpp
    src/core/nyx.h
    src/core/nyx.cpp
    src/core/rgb_triangle.h
    src/core/rgb_triangle.cpp
    src/core/sdl_wrapper.h
    src/core/sdl_wrapper.cpp
    src/core/settings.h
    src/core/settings.cpp
    src/core/vao_triangle.h
    src/core/vao_triangle.cpp
)

set(NYX_COMPUTE_KERNELS_SRC
    src/compute/kernels/addition_lattice_2d.cl
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

set(NYX_COMPUTE_SRC
    src/compute/compute_cpu.h
    src/compute/compute_cpu.cpp
    src/compute/compute_gpu.h
    src/compute/compute_gpu.cpp
    src/compute/fill_vectors.h
    src/compute/fill_vectors.cpp
    src/compute/kernel_loader.h
    src/compute/kernel_loader.cpp
    ${NYX_COMPUTE_KERNELS_SRC}
)

set(NYX_IO_SRC
    src/io/log/logger.h
    src/io/log/logger.cpp
)

set(NYX_PLATFORM_SRC
    src/platform/platform.h
    src/platform/platform.cpp
)

set(NYX_SRC
    ${NYX_CORE_SRC}
    ${NYX_COMPUTE_SRC}
    ${NYX_IO_SRC}
    ${NYX_PLATFORM_SRC}
)

# Executable
add_executable(${PROJECT_NAME} ${NYX_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# Libraries
if(Threads_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENCL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPDLOG_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
if(fmt_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

# Output
function_output_directory(${PROJECT_NAME})
file(COPY src/compute/kernels DESTINATION ${CMAKE_BINARY_DIR}/bin/)