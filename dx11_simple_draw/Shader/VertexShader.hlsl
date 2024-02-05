cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(float4 pos : POSITION,float3 normal : NORMAL,float2 texCoord : TEXCOORD0)
{
    VertexShaderOutput output;

    //  �s��̍���
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    //  �@�������[���h���W�n�ɕϊ�
    output.normal = mul(normal, (float3x3)World);

    //  �e�N�X�`�����W�����̂܂܃s�N�Z���V�F�[�_�[�ɓn��
    output.texCoord = texCoord;

    return output;
}