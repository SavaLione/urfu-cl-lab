__kernel void addition_lattice_2d(__global const float2 *a, __global const float2 *b, __global float2 *c)
{
    int index_1 = get_global_id(0);
    int index_2 = get_global_id(1);

    c[index_1 * index_2] = a[index_1 * index_2] + b[index_1 * index_2];
};