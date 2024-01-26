#pragma once

#include <DirectXMath.h>
#include "Transform.h"
#include "TransformDebugView.h"
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
    Model(std::string name);
    Model(std::string name, Transform transform, BaseHitDetection* hitDetection);


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

    ID3D11ShaderResourceView* GetTexture()  {
        return m_textureView.Get();
    }

    TransformDebugView& GetTransformView() {
        return *m_pTransformView;
    }

    BaseHitDetection* GetHitDetection() {
        return m_hitDetection;
    }

    std::string GetName() {
        return m_name;
    }

    XMVECTOR GetCenter();

    //  Setter
    void SetTexture(ID3D11ShaderResourceView* textureView) {
        m_textureView = textureView;
    }

    void SetHitDetection(BaseHitDetection* hitDetection) {
        m_hitDetection = hitDetection;
    }

    void SetName(std::string name) {
        m_name = name;
    }

private:
    //  Bufferの生成
    HRESULT CreateVertexBuffer(ID3D11Device& device);
    HRESULT CreateIndexBuffer(ID3D11Device& device);

    Transform m_transform;
    TransformDebugView* m_pTransformView;

    //  モデル名(表示に使う)
    std::string m_name;

    //  テクスチャ
    ComPtr<ID3D11ShaderResourceView> m_textureView;

    //  当たり判定
    BaseHitDetection* m_hitDetection;
};