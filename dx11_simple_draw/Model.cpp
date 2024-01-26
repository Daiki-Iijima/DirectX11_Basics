#include "Common/pch.h"
#include "Model.h"

Model::Model():IndiceCount(0) {
    vertices = std::vector<Vertex>();
    indices = std::vector<unsigned short>();
    vertexBuffer = nullptr;
    indexBuffer = nullptr;
    m_name = "no name";
    m_transform = Transform();
    m_pTransformView = new TransformDebugView(m_transform);
    m_hitDetection = nullptr;
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}

Model::Model(std::string name,Transform transform,BaseHitDetection* hitDetection) :Model() {
    this->m_name = name;
    m_hitDetection = hitDetection;
    this->m_transform = transform;
}


XMVECTOR Model::GetCenter() {
    XMFLOAT3 center = XMFLOAT3(0, 0, 0);

    for (Vertex vertex : vertices) {
        center.x += vertex.position.x;
        center.y += vertex.position.y;
        center.z += vertex.position.z;
    }

    center.x /= vertices.size();
    center.y /= vertices.size();
    center.z /= vertices.size();

    return XMVectorSet(center.x, center.y, center.z, 0);
}

HRESULT Model::CreateBuffers(ID3D11Device& device) {
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
HRESULT Model::CreateVertexBuffer(ID3D11Device& device) {
    D3D11_BUFFER_DESC vertexBufferDesc = {
        static_cast<UINT>(vertices.size() * sizeof(Vertex)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0
    };

    D3D11_SUBRESOURCE_DATA vertexSubresourceData = {
        vertices.data(),
        0, 0
    };

    //   ���_�o�b�t�@�𐶐�����
    HRESULT hr = device.CreateBuffer(&vertexBufferDesc,&vertexSubresourceData,vertexBuffer.GetAddressOf());

    if (hr == S_OK) {
        OutputDebugString(L"���_�o�b�t�@�̐����ɐ������܂����B\n");
        //vertices.clear();
    }

    return hr;
}

/// <summary>
/// �C���f�b�N�X�o�b�t�@���쐬����
/// </summary>
HRESULT Model::CreateIndexBuffer(ID3D11Device& device) {
    D3D11_BUFFER_DESC indexBufferDesc = {
        static_cast<UINT>(indices.size() * sizeof(unsigned short)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,0,0
    };

    D3D11_SUBRESOURCE_DATA indexSubresourceData = {
        indices.data(),
        0,0
    };

   HRESULT hr = device.CreateBuffer(&indexBufferDesc, &indexSubresourceData, indexBuffer.GetAddressOf());

   if (hr == S_OK) {
       OutputDebugString(L"�C���f�b�N�X�o�b�t�@�̐����ɐ������܂����B\n");
       IndiceCount = static_cast<int>(indices.size());
       indices.clear();
   }
   
   return hr;
}
