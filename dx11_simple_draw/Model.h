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
    std::vector<int> IndiceCounts;

    //  === CPUメモリ ===
    //  VertexBufferの生成ができたら解放してもいい気がする
    std::vector<std::vector<Vertex>> vertices;
    std::vector<std::vector<unsigned short>> indices;

    //  === GPUメモリ ===
    std::vector<ComPtr<ID3D11Buffer>> vertexBuffer;
    std::vector<ComPtr<ID3D11Buffer>> indexBuffer;

    //  Bufferの生成
    HRESULT CreateBuffers(ID3D11Device& device);

    //  Getter
    Transform& GetTransform() {
        return m_transform;
    }

    ID3D11ShaderResourceView* GetTexture(int index) {
        return m_textureViews[index].Get();
    }

    int GetTextureCount() {
        return m_textureViews.size();
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
    void AddTexture(ID3D11ShaderResourceView* textureView) {
        m_textureViews.push_back(textureView);
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
    std::vector<ComPtr<ID3D11ShaderResourceView>> m_textureViews;

    //  当たり判定
    BaseHitDetection* m_hitDetection;
};