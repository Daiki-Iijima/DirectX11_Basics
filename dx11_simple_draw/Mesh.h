#pragma once

#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

//  ���_�\����
struct Vertex {
    DirectX::XMFLOAT3 position; //  ���_���W
    DirectX::XMFLOAT3 normal;   //  �@���x�N�g��
    DirectX::XMFLOAT2 texcoord; //  �e�N�X�`�����W
};

class Mesh
{
public:
    Mesh();
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices);

    ~Mesh();

    HRESULT CreateBuffer(ID3D11Device& device);

    std::vector<Vertex>& GetVertices() {
        return m_pVertices;
    }

    std::vector<unsigned short>& GetIndices() {
        return m_pIndices;
    }

    //  �`��
    //  �P��ӔC�̌����Ɉᔽ���邪�A�P�����̂��߂ɂ����ɕ`�揈���������Ă���
    //  �v���W�F�N�g���傫���Ȃ�����A�`�揈���͕ʂ̃N���X�ɕ�������ق�������
    void Draw(ID3D11DeviceContext& deviceContext);

    DirectX::XMVECTOR GetCenter();

    void SetRenderEnabled(bool renderEnabled) {
        m_renderEnabled = renderEnabled;
    }

    bool GetRenderEnabled() {
        return m_renderEnabled;
    }

private:
    //  Buffer�̐���
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    //  === CPU������ ===
    std::vector<Vertex> m_pVertices;
    std::vector<unsigned short> m_pIndices;

    //  === GPU������ ===
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;

    //  �`��
    bool m_renderEnabled;
};

