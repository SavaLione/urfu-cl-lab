__kernel void simple_remove(
    __global const float* a,
    __global const float* b,
    __global float* c)
{
	int index = get_global_id(0);
	c[index] = a[index] - b[index];
};