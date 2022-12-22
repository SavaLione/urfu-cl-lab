cmake_minimum_required (VERSION 3.13.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(nyx
    VERSION "2.5.2"
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
find_package(glm REQUIRED)

if(fmt_FOUND)
    set(NYX_EXTERNAL_FMT ON)
else()
    set(NYX_EXTERNAL_FMT OFF)
endif()

# CMake configure
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.h)

# Sources
set(NYX_CORE_SRC
    src/core/execution_time.cpp
    src/core/nyx.cpp
    src/core/settings.cpp
)

set(NYX_GUI_SRC
    src/gui/buffer_representation.cpp
    src/gui/cl_image.cpp
    src/gui/cl_mandelbrot.cpp
    src/gui/cl_particles.cpp
    src/gui/gl_image.cpp
    src/gui/gl_matrices.cpp
    src/gui/image_representation.cpp
    src/gui/interop.cpp
    "src/gui/program.cpp"
    src/gui/rgb_triangle.cpp
    src/gui/sdl_wrapper.cpp
    src/gui/shader.cpp
    src/gui/vao_triangle.cpp
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
    src/compute/compute_cpu.cpp
    src/compute/compute_gpu.cpp
    src/compute/fill_vectors.cpp
    src/compute/new_gpu.cpp
    ${NYX_COMPUTE_KERNELS_SRC}
)

set(NYX_IO_SRC
    src/io/log/logger.cpp
    src/io/kernel_loader.cpp
)

set(NYX_PLATFORM_SRC
    src/platform/compiler_version.cpp
    src/platform/platform.cpp
)

set(NYX_SRC
    ${NYX_CORE_SRC}
    ${NYX_GUI_SRC}
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
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# Output
function_output_directory(${PROJECT_NAME})
file(COPY src/compute/kernels DESTINATION ${CMAKE_BINARY_DIR}/bin/)