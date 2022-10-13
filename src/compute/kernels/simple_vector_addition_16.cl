__kernel void simple_vector_addition_16(
    __global const float16* a,
    __global const float16* b,
    __global float16* c)
{
	int index = get_global_id(0);

    c[index] = a[index] + b[index];
};