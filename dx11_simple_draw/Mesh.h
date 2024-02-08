#pragma once

#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

//  頂点構造体
struct Vertex {
    DirectX::XMFLOAT3 position; //  頂点座標
    DirectX::XMFLOAT3 normal;   //  法線ベクトル
    DirectX::XMFLOAT2 texcoord; //  テクスチャ座標
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

    //  描画
    //  単一責任の原則に違反するが、単純化のためにここに描画処理を書いている
    //  プロジェクトが大きくなったら、描画処理は別のクラスに分離するほうがいい
    void Draw(ID3D11DeviceContext& deviceContext);

    DirectX::XMVECTOR GetCenter();

    void SetRenderEnabled(bool renderEnabled) {
        m_renderEnabled = renderEnabled;
    }

    bool GetRenderEnabled() {
        return m_renderEnabled;
    }

private:
    //  Bufferの生成
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    //  === CPUメモリ ===
    std::vector<Vertex> m_pVertices;
    std::vector<unsigned short> m_pIndices;

    //  === GPUメモリ ===
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;

    //  描画
    bool m_renderEnabled;
};

