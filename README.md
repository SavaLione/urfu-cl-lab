# OpenCL
## Example output
```
[2022-10-12 18:54:25.671] [info] Kernel simple_add loaded.
[2022-10-12 18:54:25.672] [info] Kernel simple_divide loaded.
[2022-10-12 18:54:25.672] [info] Kernel simple_exponentiation loaded.
[2022-10-12 18:54:25.672] [info] Kernel simple_multiple loaded.
[2022-10-12 18:54:25.672] [info] Kernel simple_remove loaded.
[2022-10-12 18:54:25.821] [info] vec_a size: 102400000
[2022-10-12 18:54:25.821] [info] vec_b size: 102400000
[2022-10-12 18:54:25.821] [info] vec_c size: 102400000
[2022-10-12 18:54:25.875] [info] Time to parallel fill vectors: 54256400 (nanoseconds)
[2022-10-12 18:54:25.875] [info] Time to parallel fill vectors: 54 (milliseconds)
[2022-10-12 18:54:31.170] [info] Time to compute vec_c on cpu: 5294819200 (nanoseconds)
[2022-10-12 18:54:31.170] [info] Time to compute vec_c on cpu: 5294 (milliseconds)
[2022-10-12 18:54:35.923] [info] Time to parallel compute vec_c on cpu: 4753161000 (nanoseconds)
[2022-10-12 18:54:35.923] [info] Time to parallel compute vec_c on cpu: 4753 (milliseconds)
[2022-10-12 18:54:35.928] [info] CPU check: 0.000000 3.500000 4679.500000 14336.000000 358400000.000000
[2022-10-12 18:54:35.928] [info] New GPU task started.
[2022-10-12 18:54:36.025] [info] Using OpenCL platform: NVIDIA CUDA
[2022-10-12 18:54:36.025] [info] Using OpenCL device: NVIDIA GeForce GTX 1060 3GB
[2022-10-12 18:54:36.966] [info] OpenGL kernel work group size: 256
[2022-10-12 18:54:37.943] [info] Time to parallel compute vec_c on gpu: 835018000 (nanoseconds)
[2022-10-12 18:54:37.943] [info] Time to parallel compute vec_c on gpu: 835 (milliseconds)
[2022-10-12 18:54:37.943] [info] GPU check: 0.000000 3.500000 4679.500000 14336.000000 358400000.000000
```
