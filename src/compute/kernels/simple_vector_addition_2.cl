__kernel void simple_vector_addition_2(
    __global const float2* a,
    __global const float2* b,
    __global float2* c)
{
	int index = get_global_id(0);

    c[index] = a[index] + b[index];
};