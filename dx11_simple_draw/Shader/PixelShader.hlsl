Texture2D tex : register(t0); // テクスチャリソース
SamplerState texSampler : register(s0); // テクスチャサンプラー

cbuffer LightBuffer : register(b1)
{
    float3 lightDirection;  // 光の方向
    float3 lightColor;      // 光の色
    float lightIntensity;   // 光の強度
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

    // ディフューズライティングの計算
    float diffuse = max(dot(normal, -normalize(lightDirection)), .1f);
    float3 diffuseColor = diffuse * lightColor * lightIntensity;

    // アンビエントライティングの計算
    float3 ambientLightColor = float3(0.1, 0.1, 0.1); // アンビエント光の色
    float ambientIntensity = 1; // アンビエント光の強度
    float3 ambientColor = ambientLightColor * ambientIntensity;

    // テクスチャから色を取得
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    // テクスチャ色とライティングの色を組み合わせる（アンビエント光を加える）
    float3 finalColor = texColor.rgb * (diffuseColor + ambientColor);

    return float4(finalColor, texColor.a);
}
