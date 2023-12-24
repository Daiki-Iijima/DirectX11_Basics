cbuffer ConstantBuffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

VertexShaderOutput main(float4 pos : POSITION,float3 normal : NORMAL)
{
    VertexShaderOutput output;

    //  �s��̍���
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    //  �@�������[���h���W�n�ɕϊ�
    output.normal = mul(normal, (float3x3)World);

    return output;
}