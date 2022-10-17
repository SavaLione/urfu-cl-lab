__kernel void multiple_vector_8(
    __global const float8* a,
    __global const float8* b,
    __global float8* c)
{
	int index = get_global_id(0);
	c[index] = a[index] * b[index];
};