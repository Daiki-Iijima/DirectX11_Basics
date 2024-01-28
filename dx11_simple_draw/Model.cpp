#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    IndiceCounts = std::vector<int>();
    vertices = std::vector<std::vector<Vertex>>();
    indices = std::vector<std::vector<unsigned short>>();
    vertexBuffer = std::vector<ComPtr<ID3D11Buffer>>();
    indexBuffer = std::vector<ComPtr<ID3D11Buffer>>();
    m_name = "no name";
    m_transform = Transform();
    m_pTransformView = new TransformDebugView(m_transform);
    m_hitDetection = nullptr;
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}

Model::Model(std::string name, Transform transform, BaseHitDetection* hitDetection) :Model() {
    this->m_name = name;
    m_hitDetection = hitDetection;
    this->m_transform = transform;
}


XMVECTOR Model::GetCenter() {
    XMFLOAT3 center = XMFLOAT3(0, 0, 0);

    for (Vertex vertex : vertices[1]) {
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
/// 頂点バッファを作成する
/// </summary>
HRESULT Model::CreateVertexBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;
    for (int i = 0; i < vertices.size(); i++) {
        const std::vector<Vertex>& meshVertices = vertices[i];
        vertexBuffer.push_back(ComPtr<ID3D11Buffer>());
        vertexBuffer[i] = nullptr;

        D3D11_BUFFER_DESC vertexBufferDesc = {
            static_cast<UINT>(meshVertices.size() * sizeof(Vertex)),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_VERTEX_BUFFER,
            0, 0, 0
        };

        D3D11_SUBRESOURCE_DATA vertexSubresourceData = {
            meshVertices.data(),
            0, 0
        };

        //   頂点バッファを生成する
        hr = device.CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, vertexBuffer[i].GetAddressOf());

        if (hr == S_OK) {
            OutputDebugString(L"頂点バッファの生成に成功しました。: " + i);
            OutputDebugString(L"\n");
        }
        else {
            OutputDebugString(L"頂点バッファの生成に失敗しました。: " + i);
            OutputDebugString(L"\n");
            break;
        }
    }
    return hr;
}

/// <summary>
/// インデックスバッファを作成する
/// </summary>
HRESULT Model::CreateIndexBuffer(ID3D11Device& device) {
    HRESULT hr = S_OK;
    for (int i = 0; i < indices.size(); i++) {
        const std::vector<unsigned short>& meshIndices = indices[i];
        indexBuffer.push_back(ComPtr<ID3D11Buffer>());
        indexBuffer[i] = nullptr;

        D3D11_BUFFER_DESC indexBufferDesc = {
            static_cast<UINT>(meshIndices.size() * sizeof(unsigned short)),
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_INDEX_BUFFER,
            0,0,0
        };

        D3D11_SUBRESOURCE_DATA indexSubresourceData = {
            meshIndices.data(),
            0,0
        };

        hr = device.CreateBuffer(&indexBufferDesc, &indexSubresourceData, indexBuffer[i].GetAddressOf());

        if (hr == S_OK) {
            OutputDebugString(L"頂点バッファの生成に成功しました。: " + i);
            OutputDebugString(L"\n");
            IndiceCounts.push_back(static_cast<int>(meshIndices.size()));
        }
        else {
            OutputDebugString(L"頂点バッファの生成に失敗しました。: " + i);
            OutputDebugString(L"\n");
            break;
        }

    }
    return hr;
}
