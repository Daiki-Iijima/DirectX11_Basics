#pragma once

#include <DirectXMath.h>
#include "Transform.h"
#include "HitDetection/BaseHitDetection.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texcoord;
};

class Model
{
public:
    Model();
    Model(Transform transform,BaseHitDetection* hitDetection);

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

    ID3D11ShaderResourceView* GetTexture()  {
        return m_textureView.Get();
    }

    BaseHitDetection* GetHitDetection() {
        return m_hitDetection;
    }

    XMVECTOR GetCenter();

    //  Setter
    void SetTexture(ID3D11ShaderResourceView* textureView) {
        m_textureView = textureView;
    }

    void SetHitDetection(BaseHitDetection* hitDetection) {
        m_hitDetection = hitDetection;
    }

private:
    //  Buffer�̐���
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    Transform m_transform;

    //  �e�N�X�`��
    ComPtr<ID3D11ShaderResourceView> m_textureView;

    //  �����蔻��
    BaseHitDetection* m_hitDetection;
};