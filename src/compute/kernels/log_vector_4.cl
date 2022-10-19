__kernel void log_vector_4(
    __global const float4* a,
    __global float4* c)
{
	int index = get_global_id(0);
	c[index] = log(a[index]);
};