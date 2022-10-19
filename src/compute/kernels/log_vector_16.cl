__kernel void log_vector_16(
    __global const float16* a,
    __global float16* c)
{
	int index = get_global_id(0);
	c[index] = log(a[index]);
};