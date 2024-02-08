#include "Common/pch.h"
#include "Mesh.h"
#include <string>

Mesh::Mesh() :
    m_pVertices(std::vector<Vertex>()), m_pIndices(std::vector<unsigned short>()),
    m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_renderEnabled(true) {
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices) :
    m_pVertices(vertices), m_pIndices(indices),
    m_vertexBuffer(nullptr), m_indexBuffer(nullptr),m_renderEnabled(true) {
}

Mesh::~Mesh() {

    m_pVertices.clear();
    m_pVertices.shrink_to_fit();

    m_pIndices.clear();
    m_pIndices.shrink_to_fit();

    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
}

DirectX::XMVECTOR Mesh::GetCenter() {
    DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0);

    for (Vertex vertex : m_pVertices) {
        center.x += vertex.position.x;
        center.y += vertex.position.y;
        center.z += vertex.position.z;
    }

    center.x /= m_pVertices.size();
    center.y /= m_pVertices.size();
    center.z /= m_pVertices.size();

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
/// 頂点バッファを作成する
/// </summary>
HRESULT Mesh::CreateVertexBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;
    for (int i = 0; i < m_pVertices.size(); i++) {

        D3D11_BUFFER_DESC vertexBufferDesc = {
            static_cast<UINT>(m_pVertices.size() * sizeof(Vertex)),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_VERTEX_BUFFER,
            0, 0, 0
        };

        D3D11_SUBRESOURCE_DATA vertexSubresourceData = {
            m_pVertices.data(),
            0, 0
        };

        //   頂点バッファを生成する
        hr = device.CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, m_vertexBuffer.GetAddressOf());

        if (hr != S_OK) {
            std::wstring message = L"頂点バッファの生成に失敗しました。: " + std::to_wstring(i);
            OutputDebugString(message.c_str());
            OutputDebugString(L"\n");
            break;
        }
    }

    return hr;
}

/// <summary>
/// インデックスバッファを作成する
/// </summary>
HRESULT Mesh::CreateIndexBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC indexBufferDesc = {
        static_cast<UINT>(m_pIndices.size() * sizeof(unsigned short)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,0,0
    };

    D3D11_SUBRESOURCE_DATA indexSubresourceData = {
        m_pIndices.data(),
        0,0
    };

    hr = device.CreateBuffer(&indexBufferDesc, &indexSubresourceData, m_indexBuffer.GetAddressOf());

    if (hr != S_OK) {
        OutputDebugString(L"頂点バッファの生成に失敗しました。\n");
    }
    return hr;
}

void Mesh::Draw(ID3D11DeviceContext& deviceContext) {
    if (!m_renderEnabled) {
        return;
    }
    //  とりあえず受け取る
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    deviceContext.IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext.IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext.DrawIndexed(m_pIndices.size(), 0, 0);
}
