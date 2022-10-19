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
[2022-10-19 20:57:25.032] [info] Using OpenCL platform: NVIDIA CUDA
[2022-10-19 20:57:25.032] [info] Using OpenCL device: NVIDIA GeForce GTX 1060 3GB
[2022-10-19 20:57:25.109] [info] Platform name: NVIDIA CUDA
[2022-10-19 20:57:25.109] [info] Platform vendor: NVIDIA Corporation
[2022-10-19 20:57:25.109] [info] Platform extensions: cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32
[2022-10-19 20:57:25.109] [info] Platform profile: FULL_PROFILE
[2022-10-19 20:57:25.109] [info] Platform version: OpenCL 3.0 CUDA 11.7.101
[2022-10-19 20:57:25.109] [info] Device name: NVIDIA GeForce GTX 1060 3GB
[2022-10-19 20:57:25.109] [info] Device vendor: NVIDIA Corporation
[2022-10-19 20:57:25.109] [info] Device version: OpenCL 3.0 CUDA
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_BUILT_IN_KERNELS:
[2022-10-19 20:57:25.109] [info] Device extensions: cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_PROFILE: FULL_PROFILE
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_MAX_CLOCK_FREQUENCY: 1771
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_MAX_MEM_ALLOC_SIZE: 805240832
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_GLOBAL_MEM_SIZE: 3220963328
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_LOCAL_MEM_SIZE: 49152
[2022-10-19 20:57:25.109] [info] Device CL_DEVICE_AVAILABLE: 1
[2022-10-19 20:57:25.321] [info] OpenCL application: addition_vector_16
[2022-10-19 20:57:26.672] [info] Time to parallel compute on gpu: 799629300 (nanoseconds)
[2022-10-19 20:57:26.672] [info] Time to parallel compute on gpu: 799 (milliseconds)
[2022-10-19 20:57:27.080] [info] OpenCL application: addition_vector_8
[2022-10-19 20:57:28.402] [info] Time to parallel compute on gpu: 780813900 (nanoseconds)
[2022-10-19 20:57:28.402] [info] Time to parallel compute on gpu: 780 (milliseconds)
[2022-10-19 20:57:28.797] [info] OpenCL application: addition_vector_4
[2022-10-19 20:57:30.094] [info] Time to parallel compute on gpu: 788415100 (nanoseconds)
[2022-10-19 20:57:30.094] [info] Time to parallel compute on gpu: 788 (milliseconds)
[2022-10-19 20:57:30.569] [info] OpenCL application: addition_vector_2
[2022-10-19 20:57:31.947] [info] Time to parallel compute on gpu: 789137900 (nanoseconds)
[2022-10-19 20:57:31.947] [info] Time to parallel compute on gpu: 789 (milliseconds)
[2022-10-19 20:57:32.370] [info] OpenCL application: divide_vector_16
[2022-10-19 20:57:33.859] [info] Time to parallel compute on gpu: 957627200 (nanoseconds)
[2022-10-19 20:57:33.859] [info] Time to parallel compute on gpu: 957 (milliseconds)
[2022-10-19 20:57:34.278] [info] OpenCL application: divide_vector_8
[2022-10-19 20:57:35.593] [info] Time to parallel compute on gpu: 780056300 (nanoseconds)
[2022-10-19 20:57:35.593] [info] Time to parallel compute on gpu: 780 (milliseconds)
[2022-10-19 20:57:35.978] [info] OpenCL application: divide_vector_4
[2022-10-19 20:57:37.301] [info] Time to parallel compute on gpu: 781844800 (nanoseconds)
[2022-10-19 20:57:37.301] [info] Time to parallel compute on gpu: 781 (milliseconds)
[2022-10-19 20:57:37.734] [info] OpenCL application: divide_vector_2
[2022-10-19 20:57:39.035] [info] Time to parallel compute on gpu: 779482600 (nanoseconds)
[2022-10-19 20:57:39.035] [info] Time to parallel compute on gpu: 779 (milliseconds)
[2022-10-19 20:57:39.451] [info] OpenCL application: exponentiation_vector_16
[2022-10-19 20:57:40.897] [info] Time to parallel compute on gpu: 908320800 (nanoseconds)
[2022-10-19 20:57:40.897] [info] Time to parallel compute on gpu: 908 (milliseconds)
[2022-10-19 20:57:41.315] [info] OpenCL application: exponentiation_vector_8
[2022-10-19 20:57:42.630] [info] Time to parallel compute on gpu: 781139700 (nanoseconds)
[2022-10-19 20:57:42.630] [info] Time to parallel compute on gpu: 781 (milliseconds)
[2022-10-19 20:57:43.019] [info] OpenCL application: exponentiation_vector_4
[2022-10-19 20:57:44.336] [info] Time to parallel compute on gpu: 781237100 (nanoseconds)
[2022-10-19 20:57:44.336] [info] Time to parallel compute on gpu: 781 (milliseconds)
[2022-10-19 20:57:44.725] [info] OpenCL application: exponentiation_vector_2
[2022-10-19 20:57:46.026] [info] Time to parallel compute on gpu: 778248800 (nanoseconds)
[2022-10-19 20:57:46.026] [info] Time to parallel compute on gpu: 778 (milliseconds)
[2022-10-19 20:57:46.433] [info] OpenCL application: multiple_vector_16
[2022-10-19 20:57:47.765] [info] Time to parallel compute on gpu: 800025000 (nanoseconds)
[2022-10-19 20:57:47.765] [info] Time to parallel compute on gpu: 800 (milliseconds)
[2022-10-19 20:57:48.177] [info] OpenCL application: multiple_vector_8
[2022-10-19 20:57:49.498] [info] Time to parallel compute on gpu: 780928300 (nanoseconds)
[2022-10-19 20:57:49.498] [info] Time to parallel compute on gpu: 780 (milliseconds)
[2022-10-19 20:57:49.881] [info] OpenCL application: multiple_vector_4
[2022-10-19 20:57:51.197] [info] Time to parallel compute on gpu: 781411600 (nanoseconds)
[2022-10-19 20:57:51.197] [info] Time to parallel compute on gpu: 781 (milliseconds)
[2022-10-19 20:57:51.588] [info] OpenCL application: multiple_vector_2
[2022-10-19 20:57:52.894] [info] Time to parallel compute on gpu: 777546500 (nanoseconds)
[2022-10-19 20:57:52.894] [info] Time to parallel compute on gpu: 777 (milliseconds)
[2022-10-19 20:57:53.305] [info] OpenCL application: remove_vector_16
[2022-10-19 20:57:54.642] [info] Time to parallel compute on gpu: 799578100 (nanoseconds)
[2022-10-19 20:57:54.642] [info] Time to parallel compute on gpu: 799 (milliseconds)
[2022-10-19 20:57:55.053] [info] OpenCL application: remove_vector_8
[2022-10-19 20:57:56.367] [info] Time to parallel compute on gpu: 781476900 (nanoseconds)
[2022-10-19 20:57:56.367] [info] Time to parallel compute on gpu: 781 (milliseconds)
[2022-10-19 20:57:56.752] [info] OpenCL application: remove_vector_4
[2022-10-19 20:57:58.058] [info] Time to parallel compute on gpu: 780239000 (nanoseconds)
[2022-10-19 20:57:58.058] [info] Time to parallel compute on gpu: 780 (milliseconds)
[2022-10-19 20:57:58.460] [info] OpenCL application: remove_vector_2
[2022-10-19 20:57:59.768] [info] Time to parallel compute on gpu: 779040300 (nanoseconds)
[2022-10-19 20:57:59.768] [info] Time to parallel compute on gpu: 779 (milliseconds)
[2022-10-19 20:58:00.093] [info] OpenCL application: log_vector_16
[2022-10-19 20:58:00.977] [info] Time to parallel compute on gpu: 541756700 (nanoseconds)
[2022-10-19 20:58:00.977] [info] Time to parallel compute on gpu: 541 (milliseconds)
[2022-10-19 20:58:01.230] [info] OpenCL application: log_vector_8
[2022-10-19 20:58:01.844] [info] Time to parallel compute on gpu: 274254700 (nanoseconds)
[2022-10-19 20:58:01.844] [info] Time to parallel compute on gpu: 274 (milliseconds)
[2022-10-19 20:58:02.084] [info] OpenCL application: log_vector_4
[2022-10-19 20:58:02.580] [info] Time to parallel compute on gpu: 143081200 (nanoseconds)
[2022-10-19 20:58:02.580] [info] Time to parallel compute on gpu: 143 (milliseconds)
[2022-10-19 20:58:02.842] [info] OpenCL application: log_vector_2
[2022-10-19 20:58:03.248] [info] Time to parallel compute on gpu: 78883000 (nanoseconds)
[2022-10-19 20:58:03.248] [info] Time to parallel compute on gpu: 78 (milliseconds)
```
