Texture2D tex : register(t0); // �e�N�X�`�����\�[�X
SamplerState texSampler : register(s0); // �e�N�X�`���T���v���[

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
    // ���̕���
    float3 lightDirection = normalize(float3(0.0f, 0.0f, -1.0));
    // ���̐F
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);

    // �@���̐��K��
    float3 normal = normalize(input.normal);

    // ���C�e�B���O�̌v�Z
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    float3 color = diffuse * lightColor;

    // �e�N�X�`������F���擾
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    // �e�N�X�`���F�ƃ��C�e�B���O�̐F��g�ݍ��킹��
    float3 finalColor = texColor.rgb * color;

    return float4(finalColor, texColor.a);
}
