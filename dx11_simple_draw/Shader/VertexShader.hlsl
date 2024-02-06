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

    //  行列の合成
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    //  法線をワールド座標系に変換
    output.normal = mul(normal, (float3x3)World);

    //  テクスチャ座標をそのままピクセルシェーダーに渡す
    output.texCoord = texCoord;

    return output;
}