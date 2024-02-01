#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    m_name = "no name";
    m_componentViews = std::vector<IComponentUIDebugView*>();
    m_transform = Transform();
    m_componentViews.push_back(new TransformUIDebugView(m_transform));
    m_pMesh = Mesh();
    m_componentViews.push_back(new MeshUIDebugView(m_pMesh));
    m_hitDetection = nullptr;
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
    m_pParent = nullptr;
    m_pChilds = std::vector<Model*>();
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}

Model::Model(std::string name, Transform transform, BaseHitDetection* hitDetection) :Model() {
    this->m_name = name;
    m_hitDetection = hitDetection;
    this->m_transform = transform;
}
