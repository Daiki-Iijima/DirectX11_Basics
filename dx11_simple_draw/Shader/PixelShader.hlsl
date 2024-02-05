Texture2D tex : register(t0); // テクスチャリソース
SamplerState texSampler : register(s0); // テクスチャサンプラー

cbuffer LightBuffer : register(b1)
{
    float3 lightDirection;  //  光の方向
    float3 lightColor;      //  光の色
    float lightIntensity;   //  光の強度
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
    // 法線の正規化
    float3 normal = normalize(input.normal);

    // ライティングの計算
    float diffuse = max(dot(normal, -normalize(lightDirection)), 0.0f);
    float3 color = diffuse * lightColor * lightIntensity;

    // テクスチャから色を取得
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    // テクスチャ色とライティングの色を組み合わせる
    float3 finalColor = texColor.rgb * color;

    return float4(finalColor, texColor.a);
}
