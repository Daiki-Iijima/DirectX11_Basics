#pragma once

#include <DirectXMath.h>
#include "Transform.h"
#include "TransformUIDebugView.h"
#include "HitDetection/BaseHitDetection.h"
#include "Mesh.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Model
{
public:
    Model();
    Model(std::string name);
    Model(std::string name, Transform transform, BaseHitDetection* hitDetection);

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

    TransformUIDebugView& GetTransformView() {
        return *m_pTransformView;
    }

    BaseHitDetection* GetHitDetection() {
        return m_hitDetection;
    }

    std::string GetName() {
        return m_name;
    }

    Mesh& GetMesh() {
        return *m_pMesh;
    }

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

    void SetMesh(Mesh* mesh) {
        m_pMesh = mesh;
    }


private:
    //  モデル名(表示に使う)
    std::string m_name;

    Mesh* m_pMesh;

    Transform m_transform;
    TransformUIDebugView* m_pTransformView;

    //  親子関係
    Model* m_pParent;               //  親
    std::vector<Model*> m_pChilds;  //  子

    //  テクスチャ
    std::vector<ComPtr<ID3D11ShaderResourceView>> m_textureViews;

    //  当たり判定
    BaseHitDetection* m_hitDetection;
};