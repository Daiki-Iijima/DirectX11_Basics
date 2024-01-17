#include "Common/pch.h"
#include "Model.h"

Model::Model() {
    vertices = std::vector<Vertex>();
    indices = std::vector<unsigned short>();
    vertexBuffer = nullptr;
    indexBuffer = nullptr;
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
/// 頂点バッファを作成する
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

    //   頂点バッファを生成する
    HRESULT hr = device.CreateBuffer(&vertexBufferDesc,&vertexSubresourceData,vertexBuffer.GetAddressOf());

    if (hr == S_OK) {
        OutputDebugString(L"頂点バッファの生成に成功しました。\n");
        vertices.clear();
    }

    return hr;
}

/// <summary>
/// インデックスバッファを作成する
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
       OutputDebugString(L"インデックスバッファの生成に成功しました。\n");
       IndiceCount = static_cast<int>(indices.size());
       indices.clear();
   }
   
   return hr;
}
