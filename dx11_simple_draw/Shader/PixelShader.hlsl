Texture2D tex : register(t0); // �e�N�X�`�����\�[�X
SamplerState texSampler : register(s0); // �e�N�X�`���T���v���[

cbuffer LightBuffer : register(b1)
{
    float3 lightDirection;  // ���̕���
    float3 lightColor;      // ���̐F
    float lightIntensity;   // ���̋��x
};


struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
    // �@���̐��K��
    float3 normal = normalize(input.normal);

    // �f�B�t���[�Y���C�e�B���O�̌v�Z
    float diffuse = max(dot(normal, -normalize(lightDirection)), .1f);
    float3 diffuseColor = diffuse * lightColor * lightIntensity;

    // �A���r�G���g���C�e�B���O�̌v�Z
    float3 ambientLightColor = float3(0.1, 0.1, 0.1); // �A���r�G���g���̐F
    float ambientIntensity = 1; // �A���r�G���g���̋��x
    float3 ambientColor = ambientLightColor * ambientIntensity;

    // �e�N�X�`������F���擾
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    // �e�N�X�`���F�ƃ��C�e�B���O�̐F��g�ݍ��킹��i�A���r�G���g����������j
    float3 finalColor = texColor.rgb * (diffuseColor + ambientColor);

    return float4(finalColor, texColor.a);
}
