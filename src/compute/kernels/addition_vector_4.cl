__kernel void addition_vector_4(__global const float4 *a, __global const float4 *b, __global float4 *c)
{
    int index = get_global_id(0);

    c[index] = a[index] + b[index];
};