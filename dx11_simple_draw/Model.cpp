#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    m_name = "no name";
    m_transform = Transform();
    m_componentViews.push_back(std::make_unique<TransformUIDebugView>(m_transform));
    m_pMesh = std::make_shared<Mesh>();
    m_componentViews.push_back(std::make_unique<MeshUIDebugView>(m_pMesh));
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
    m_pParent = nullptr;
    m_pChilds = std::vector<Model*>();
}

Model::~Model() {
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}

Model::Model(std::shared_ptr<Mesh>& mesh){
    m_name = "no name";
    m_transform = Transform();
    m_componentViews.push_back(std::make_unique<TransformUIDebugView>(m_transform));
    m_pMesh = mesh;
    m_componentViews.push_back(std::make_unique<MeshUIDebugView>(m_pMesh));
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
    m_pParent = nullptr;
    m_pChilds = std::vector<Model*>();
}


Model::Model(std::string name, Transform transform) : Model() {
    this->m_name = name;
    this->m_transform = transform;
}
