__kernel void simple_vec_add(
    __global const float16* a,
    __global const float16* b,
    __global float16* c)
{
	int index = get_global_id(0);

    c[index].s0 = a[index].s0 + b[index].s0;
    c[index].s1 = a[index].s1 + b[index].s1;
    c[index].s2 = a[index].s2 + b[index].s2;
    c[index].s3 = a[index].s3 + b[index].s3;
    c[index].s4 = a[index].s4 + b[index].s4;
    c[index].s5 = a[index].s5 + b[index].s5;
    c[index].s6 = a[index].s6 + b[index].s6;
    c[index].s7 = a[index].s7 + b[index].s7;
    c[index].s8 = a[index].s8 + b[index].s8;
    c[index].s9 = a[index].s9 + b[index].s9;
    c[index].sa = a[index].sa + b[index].sa;
    c[index].sb = a[index].sb + b[index].sb;
    c[index].sc = a[index].sc + b[index].sc;
    c[index].sd = a[index].sd + b[index].sd;
    c[index].se = a[index].se + b[index].se;
    c[index].sf = a[index].sf + b[index].sf;
};