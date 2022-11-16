const uchar SPEED = 1;

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void example(
    __read_only image2d_t img_in,
    __write_only image2d_t img_out,
    __global uchar* power,
    __global const uchar* direction)
{
    int2 pos = {get_global_id(0), get_global_id(1)};

    float4 pixel_ori = read_imagef(img_in, sampler, pos);
    float4 pixel_rdy = (float4)(0.0f);

    if((pos.x % 2) == 0)
    {
        pixel_rdy = pixel_ori;
    }

    write_imagef(img_out, (int2)(pos.x, pos.y), pixel_rdy);
}