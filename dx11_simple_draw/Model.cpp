#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    m_name = "no name";
    m_componentViews = std::vector<IUIDebugComponent*>();
    m_transform = Transform();
    m_componentViews.push_back(new TransformUIDebugView(m_transform));
    m_pMesh = Mesh();
    m_componentViews.push_back(new MeshUIDebugView(m_pMesh));
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
    m_pParent = nullptr;
    m_pChilds = std::vector<Model*>();
    m_components = new std::vector<IComponent*>();
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}


Model::Model(std::string name, Transform transform, std::vector<IComponent*>* components) : Model() {
    this->m_name = name;
    this->m_transform = transform;
    this->m_components = components;
}
