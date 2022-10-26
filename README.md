# OpenCL

## Tasks
1. First OpenCL lab
    1. Write OpenCL kernels
        - [x] add
        - [x] remove
        - [x] divide
        - [x] exponentiation
        - [ ] condition
        - [x] log
        - [ ] trig
        - [ ] csv table
    2. Run OpenCL kernel multiple times and reduce work group size each time
        - [ ] reduce work group size
        - [ ] save data to excel table
        - [ ] draw chart
    3. Two-dimensional lattice
    4. Show information about OpenCL device
        - [x] OpenCL version
        - [x] Device name
        - [x] Global memory size
        - [x] Local memory size
        - [x] Maximum clock frequency
        - [ ] Maximum number of work group
        - [ ] Maximum number of computational units
        - [ ] Maximum OpenCL image width and height
        - [ ] Maximum number of work group dimensions
        - [ ] Maximum number of work group per dimension

## Example output
```
[2022-10-26 19:30:25.259] [info] Compute CPU application: Addition
[2022-10-26 19:30:30.062] [info] Time to parallel compute on cpu: 4802369900 (nanoseconds)
[2022-10-26 19:30:30.062] [info] Time to parallel compute on cpu: 4802 (milliseconds)
[2022-10-26 19:30:30.062] [info] Compute CPU application: Remove
[2022-10-26 19:30:35.210] [info] Time to parallel compute on cpu: 5147535700 (nanoseconds)
[2022-10-26 19:30:35.210] [info] Time to parallel compute on cpu: 5147 (milliseconds)
[2022-10-26 19:30:35.210] [info] Compute CPU application: Multiple
[2022-10-26 19:30:39.931] [info] Time to parallel compute on cpu: 4721044000 (nanoseconds)
[2022-10-26 19:30:39.931] [info] Time to parallel compute on cpu: 4721 (milliseconds)
[2022-10-26 19:30:39.931] [info] Compute CPU application: Divide
[2022-10-26 19:30:44.648] [info] Time to parallel compute on cpu: 4717152400 (nanoseconds)
[2022-10-26 19:30:44.648] [info] Time to parallel compute on cpu: 4717 (milliseconds)
[2022-10-26 19:30:44.648] [info] Compute CPU application: Exponentiation
[2022-10-26 19:30:48.315] [info] Time to parallel compute on cpu: 3667013700 (nanoseconds)
[2022-10-26 19:30:48.315] [info] Time to parallel compute on cpu: 3667 (milliseconds)
[2022-10-26 19:30:48.315] [info] Compute CPU application: Log
[2022-10-26 19:31:51.716] [info] Time to parallel compute on cpu: 63401601300 (nanoseconds)
[2022-10-26 19:31:51.716] [info] Time to parallel compute on cpu: 63401 (milliseconds)
[2022-10-26 19:31:51.811] [info] Using OpenCL platform: NVIDIA CUDA
[2022-10-26 19:31:51.811] [info] Using OpenCL device: NVIDIA GeForce GTX 1060 3GB
[2022-10-26 19:31:51.950] [info] Platform name: NVIDIA CUDA
[2022-10-26 19:31:51.950] [info] Platform vendor: NVIDIA Corporation
[2022-10-26 19:31:51.950] [info] Platform extensions: cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32
[2022-10-26 19:31:51.950] [info] Platform profile: FULL_PROFILE
[2022-10-26 19:31:51.950] [info] Platform version: OpenCL 3.0 CUDA 11.7.101
[2022-10-26 19:31:51.950] [info] Device name: NVIDIA GeForce GTX 1060 3GB
[2022-10-26 19:31:51.950] [info] Device vendor: NVIDIA Corporation
[2022-10-26 19:31:51.950] [info] Device version: OpenCL 3.0 CUDA
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_BUILT_IN_KERNELS:
[2022-10-26 19:31:51.950] [info] Device extensions: cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_PROFILE: FULL_PROFILE
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_MAX_CLOCK_FREQUENCY: 1771
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_MAX_MEM_ALLOC_SIZE: 805240832
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_GLOBAL_MEM_SIZE: 3220963328
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_LOCAL_MEM_SIZE: 49152
[2022-10-26 19:31:51.950] [info] Device CL_DEVICE_AVAILABLE: 1
[2022-10-26 19:31:52.169] [info] OpenCL application: addition_vector_16
[2022-10-26 19:31:53.567] [info] Time to parallel compute on gpu: 844408200 (nanoseconds)
[2022-10-26 19:31:53.567] [info] Time to parallel compute on gpu: 844 (milliseconds)
[2022-10-26 19:31:53.947] [info] OpenCL application: addition_vector_8
[2022-10-26 19:31:55.279] [info] Time to parallel compute on gpu: 796006000 (nanoseconds)
[2022-10-26 19:31:55.279] [info] Time to parallel compute on gpu: 796 (milliseconds)
[2022-10-26 19:31:55.634] [info] OpenCL application: addition_vector_4
[2022-10-26 19:31:56.917] [info] Time to parallel compute on gpu: 789030100 (nanoseconds)
[2022-10-26 19:31:56.917] [info] Time to parallel compute on gpu: 789 (milliseconds)
[2022-10-26 19:31:57.254] [info] OpenCL application: addition_vector_2
[2022-10-26 19:31:58.570] [info] Time to parallel compute on gpu: 784800400 (nanoseconds)
[2022-10-26 19:31:58.570] [info] Time to parallel compute on gpu: 784 (milliseconds)
[2022-10-26 19:31:58.919] [info] OpenCL application: divide_vector_16
[2022-10-26 19:32:00.409] [info] Time to parallel compute on gpu: 966665000 (nanoseconds)
[2022-10-26 19:32:00.409] [info] Time to parallel compute on gpu: 966 (milliseconds)
[2022-10-26 19:32:00.759] [info] OpenCL application: divide_vector_8
[2022-10-26 19:32:02.043] [info] Time to parallel compute on gpu: 788707800 (nanoseconds)
[2022-10-26 19:32:02.043] [info] Time to parallel compute on gpu: 788 (milliseconds)
[2022-10-26 19:32:02.377] [info] OpenCL application: divide_vector_4
[2022-10-26 19:32:03.684] [info] Time to parallel compute on gpu: 788972600 (nanoseconds)
[2022-10-26 19:32:03.684] [info] Time to parallel compute on gpu: 788 (milliseconds)
[2022-10-26 19:32:04.014] [info] OpenCL application: divide_vector_2
[2022-10-26 19:32:05.326] [info] Time to parallel compute on gpu: 789064600 (nanoseconds)
[2022-10-26 19:32:05.326] [info] Time to parallel compute on gpu: 789 (milliseconds)
[2022-10-26 19:32:05.703] [info] OpenCL application: exponentiation_vector_16
[2022-10-26 19:32:07.111] [info] Time to parallel compute on gpu: 915823500 (nanoseconds)
[2022-10-26 19:32:07.111] [info] Time to parallel compute on gpu: 915 (milliseconds)
[2022-10-26 19:32:07.463] [info] OpenCL application: exponentiation_vector_8
[2022-10-26 19:32:08.781] [info] Time to parallel compute on gpu: 791250900 (nanoseconds)
[2022-10-26 19:32:08.781] [info] Time to parallel compute on gpu: 791 (milliseconds)
[2022-10-26 19:32:09.123] [info] OpenCL application: exponentiation_vector_4
[2022-10-26 19:32:10.442] [info] Time to parallel compute on gpu: 791490200 (nanoseconds)
[2022-10-26 19:32:10.442] [info] Time to parallel compute on gpu: 791 (milliseconds)
[2022-10-26 19:32:10.773] [info] OpenCL application: exponentiation_vector_2
[2022-10-26 19:32:12.082] [info] Time to parallel compute on gpu: 785793600 (nanoseconds)
[2022-10-26 19:32:12.082] [info] Time to parallel compute on gpu: 785 (milliseconds)
[2022-10-26 19:32:12.433] [info] OpenCL application: multiple_vector_16
[2022-10-26 19:32:13.756] [info] Time to parallel compute on gpu: 810520000 (nanoseconds)
[2022-10-26 19:32:13.756] [info] Time to parallel compute on gpu: 810 (milliseconds)
[2022-10-26 19:32:14.119] [info] OpenCL application: multiple_vector_8
[2022-10-26 19:32:15.433] [info] Time to parallel compute on gpu: 791563900 (nanoseconds)
[2022-10-26 19:32:15.433] [info] Time to parallel compute on gpu: 791 (milliseconds)
[2022-10-26 19:32:15.774] [info] OpenCL application: multiple_vector_4
[2022-10-26 19:32:17.059] [info] Time to parallel compute on gpu: 791142400 (nanoseconds)
[2022-10-26 19:32:17.059] [info] Time to parallel compute on gpu: 791 (milliseconds)
[2022-10-26 19:32:17.395] [info] OpenCL application: multiple_vector_2
[2022-10-26 19:32:18.697] [info] Time to parallel compute on gpu: 784892300 (nanoseconds)
[2022-10-26 19:32:18.697] [info] Time to parallel compute on gpu: 784 (milliseconds)
[2022-10-26 19:32:19.051] [info] OpenCL application: remove_vector_16
[2022-10-26 19:32:20.365] [info] Time to parallel compute on gpu: 807925700 (nanoseconds)
[2022-10-26 19:32:20.365] [info] Time to parallel compute on gpu: 807 (milliseconds)
[2022-10-26 19:32:20.738] [info] OpenCL application: remove_vector_8
[2022-10-26 19:32:22.024] [info] Time to parallel compute on gpu: 788366700 (nanoseconds)
[2022-10-26 19:32:22.024] [info] Time to parallel compute on gpu: 788 (milliseconds)
[2022-10-26 19:32:22.358] [info] OpenCL application: remove_vector_4
[2022-10-26 19:32:23.661] [info] Time to parallel compute on gpu: 787315200 (nanoseconds)
[2022-10-26 19:32:23.661] [info] Time to parallel compute on gpu: 787 (milliseconds)
[2022-10-26 19:32:24.026] [info] OpenCL application: remove_vector_2
[2022-10-26 19:32:25.339] [info] Time to parallel compute on gpu: 788992700 (nanoseconds)
[2022-10-26 19:32:25.339] [info] Time to parallel compute on gpu: 788 (milliseconds)
[2022-10-26 19:32:25.629] [info] OpenCL application: log_vector_16
[2022-10-26 19:32:26.513] [info] Time to parallel compute on gpu: 550016900 (nanoseconds)
[2022-10-26 19:32:26.513] [info] Time to parallel compute on gpu: 550 (milliseconds)
[2022-10-26 19:32:26.756] [info] OpenCL application: log_vector_8
[2022-10-26 19:32:27.379] [info] Time to parallel compute on gpu: 277682100 (nanoseconds)
[2022-10-26 19:32:27.379] [info] Time to parallel compute on gpu: 277 (milliseconds)
[2022-10-26 19:32:27.626] [info] OpenCL application: log_vector_4
[2022-10-26 19:32:28.129] [info] Time to parallel compute on gpu: 150238400 (nanoseconds)
[2022-10-26 19:32:28.129] [info] Time to parallel compute on gpu: 150 (milliseconds)
[2022-10-26 19:32:28.366] [info] OpenCL application: log_vector_2
[2022-10-26 19:32:28.786] [info] Time to parallel compute on gpu: 80284000 (nanoseconds)
[2022-10-26 19:32:28.786] [info] Time to parallel compute on gpu: 80 (milliseconds)

```
