cbuffer ConstantBuffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    float4 output = pos;

    output = mul(pos, World);
    output = mul(output, View);
    output = mul(output, Projection);

    return output;
}