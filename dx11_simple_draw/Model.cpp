#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    m_name = "no name";
    m_componentViews = std::vector<IUIDebugComponent*>();
    m_transform = Transform();
    m_componentViews.push_back(new TransformUIDebugView(m_transform));
    m_mesh = Mesh();
    m_componentViews.push_back(new MeshUIDebugView(m_mesh));
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
    m_pParent = nullptr;
    m_pChilds = std::vector<Model*>();
}

Model::~Model() {
    for(auto componentView : m_componentViews){
        delete componentView;
        componentView = nullptr;
    }
    m_textureViews.clear();
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}


Model::Model(std::string name, Transform transform) : Model() {
    this->m_name = name;
    this->m_transform = transform;
}
