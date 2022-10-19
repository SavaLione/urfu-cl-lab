__kernel void log_vector_2(
    __global const float2* a,
    __global float2* c)
{
	int index = get_global_id(0);
	c[index] = log(a[index]);
};