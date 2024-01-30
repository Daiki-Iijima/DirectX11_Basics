#include "Common/pch.h"
#include "Model.h"

Model::Model(){
    m_pMesh = nullptr;
    m_name = "no name";
    m_transform = Transform();
    m_pTransformView = new TransformUIDebugView(m_transform);
    m_hitDetection = nullptr;
    m_textureViews = std::vector<ComPtr<ID3D11ShaderResourceView>>();
}

Model::Model(std::string name) : Model() {
    this->m_name = name;
}

Model::Model(std::string name, Transform transform, BaseHitDetection* hitDetection) :Model() {
    this->m_name = name;
    m_hitDetection = hitDetection;
    this->m_transform = transform;
}
