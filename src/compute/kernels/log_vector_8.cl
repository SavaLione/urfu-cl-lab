__kernel void log_vector_8(__global const float8 *a, __global float8 *c)
{
    int index = get_global_id(0);
    c[index]  = log(a[index]);
};