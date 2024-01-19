#pragma once

#include <DirectXMath.h>
#include "Transform.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
};

class Model
{
public:
    Model();
    Model(Transform transform);

    //  �C���f�b�N�X�o�b�t�@�̐�
    int IndiceCount;

    //  === CPU������ ===
    //  VertexBuffer�̐������ł����������Ă������C������
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    //  === GPU������ ===
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    //  Buffer�̐���
    HRESULT CreateBuffers(ID3D11Device& device);

    //  Getter
    Transform& GetTransform() {
        return m_transform;
    }

    //  Setter
    void SetTexture(ID3D11ShaderResourceView* textureView) {
        m_textureView = textureView;
    }

private:
    //  Buffer�̐���
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    Transform m_transform;

    //  �e�N�X�`��
    ID3D11ShaderResourceView* m_textureView;
};