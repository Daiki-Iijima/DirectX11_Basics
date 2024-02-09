#pragma once

#include <DirectXMath.h>
#include "Transform.h"
#include "TransformUIDebugView.h"
#include "IComponent.h"
#include "Mesh.h"
#include "MeshUIDebugView.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Model
{
public:
    Model();
    Model(std::shared_ptr<Mesh>& mesh);
    Model(std::string name);
    Model(std::string name, Transform transform);
    ~Model();

    //  Getter
    Transform& GetTransform() {
        return m_transform;
    }

    ID3D11ShaderResourceView* GetTexture(int index) {
        return m_textureViews[index].Get();
    }

    int GetTextureCount() {
        try {
            return m_textureViews.size();
        }
        catch (std::exception e) {
            return 0;
        }
    }

    std::vector<std::unique_ptr<IUIDebugComponent>>& GetComponentUIDebugViews() {
        return m_componentViews;
    }

    std::string GetName() {
        return m_name;
    }

    std::shared_ptr<Mesh>& GetMesh() {
        return m_pMesh;
    }

    //  Setter
    void AddTexture(ID3D11ShaderResourceView* textureView) {
        m_textureViews.push_back(textureView);
    }

    void SetName(std::string name) {
        m_name = name;
    }

    void AddComponent(std::shared_ptr<IComponent> component) {
        m_components.push_back(std::move(component));
    }

    std::vector<std::shared_ptr<IComponent>> GetComponents() {
        return m_components;
    }

private:
    //  ���f����(�\���Ɏg��)
    std::string m_name;

    Transform m_transform;
    std::shared_ptr<Mesh> m_pMesh;

    std::vector<std::unique_ptr<IUIDebugComponent>> m_componentViews;

    //  �e�q�֌W
    Model* m_pParent;               //  �e
    std::vector<Model*> m_pChilds;  //  �q

    //  �e�N�X�`��
    std::vector<ComPtr<ID3D11ShaderResourceView>> m_textureViews;

    //  ���̃��f���ɕR�Â��Ă���R���|�[�l���g
    std::vector<std::shared_ptr<IComponent>> m_components;
};