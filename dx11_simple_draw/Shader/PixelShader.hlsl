struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
    //  ���̕���
    float3 lightDirection = normalize(float3(0.0f, 0.0f, -1.0));
    //  ���̐F
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);

    //  �@���̐��K��
    float3 normal = normalize(input.normal);

    //  ���C�e�B���O�̌v�Z
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    float3 color = diffuse * lightColor;

    return float4(color, 1.0f);
}