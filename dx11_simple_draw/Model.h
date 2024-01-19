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

    //  インデックスバッファの数
    int IndiceCount;

    //  === CPUメモリ ===
    //  VertexBufferの生成ができたら解放してもいい気がする
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    //  === GPUメモリ ===
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    //  Bufferの生成
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
    //  Bufferの生成
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    Transform m_transform;

    //  テクスチャ
    ID3D11ShaderResourceView* m_textureView;
};