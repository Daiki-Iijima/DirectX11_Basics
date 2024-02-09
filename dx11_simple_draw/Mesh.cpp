#include "Common/pch.h"
#include "Mesh.h"
#include <string>

Mesh::Mesh() :
    m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_renderEnabled(true) {
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices) :
    m_vertexBuffer(nullptr), m_indexBuffer(nullptr),m_renderEnabled(true) {
}

Mesh::~Mesh() {
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
}

DirectX::XMVECTOR Mesh::GetCenter() {
    DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0);

    for (Vertex vertex : *m_pVertices.get()) {
        center.x += vertex.position.x;
        center.y += vertex.position.y;
        center.z += vertex.position.z;
    }

    center.x /= m_pVertices.get()->size();
    center.y /= m_pVertices.get()->size();
    center.z /= m_pVertices.get()->size();

    return DirectX::XMVectorSet(center.x, center.y, center.z, 0);
}

HRESULT Mesh::CreateBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;

    hr = CreateVertexBuffer(device);
    if (FAILED(hr)) {
        return hr;
    }

    hr = CreateIndexBuffer(device);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}

/// <summary>
/// ���_�o�b�t�@���쐬����
/// </summary>
HRESULT Mesh::CreateVertexBuffer(ID3D11Device& device) {
    if (m_pVertices == nullptr || m_pVertices.get()->empty()) {
        return E_FAIL; // �������^�[��: ���_�f�[�^���Ȃ��ꍇ�͎��s��Ԃ�
    }

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = static_cast<UINT>(m_pVertices.get()->size() * sizeof(Vertex)); // �S���_�f�[�^�̃T�C�Y
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = {};
    vertexSubresourceData.pSysMem = m_pVertices.get()->data(); // ���_�f�[�^�̃|�C���^
    vertexSubresourceData.SysMemPitch = 0;
    vertexSubresourceData.SysMemSlicePitch = 0;

    // ���_�o�b�t�@����x������������
    HRESULT hr = device.CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr)) {
        OutputDebugString(L"���_�o�b�t�@�̐����Ɏ��s���܂����B\n");
    }

    return hr;
}

/// <summary>
/// �C���f�b�N�X�o�b�t�@���쐬����
/// </summary>
HRESULT Mesh::CreateIndexBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC indexBufferDesc = {
        static_cast<UINT>(m_pIndices->size() * sizeof(unsigned short)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,0,0
    };

    D3D11_SUBRESOURCE_DATA indexSubresourceData = {
        m_pIndices->data(),
        0,0
    };

    hr = device.CreateBuffer(&indexBufferDesc, &indexSubresourceData, m_indexBuffer.GetAddressOf());

    if (hr != S_OK) {
        OutputDebugString(L"���_�o�b�t�@�̐����Ɏ��s���܂����B\n");
    }
    return hr;
}

void Mesh::Draw(ID3D11DeviceContext& deviceContext) {
    if (!m_renderEnabled) {
        return;
    }
    //  �Ƃ肠�����󂯎��
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    deviceContext.IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext.IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext.DrawIndexed(m_pIndices->size(), 0, 0);
}
