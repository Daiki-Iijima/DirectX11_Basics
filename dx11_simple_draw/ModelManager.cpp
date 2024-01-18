#include "Common/pch.h"
#include "ModelManager.h"

using namespace DirectX;

void ModelManager::LoadModel(Model& distModel, string modelPath) {
    //  モデルの読み込み
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals  // 法線情報の計算を追加
    );

    if (!scene) {
        // エラーハンドリング
        std::string errorStr = "ERROR: " + std::string(importer.GetErrorString()) + "\n";
        std::wstring errorWStr = std::wstring(errorStr.begin(), errorStr.end());
        OutputDebugString(errorWStr.c_str());
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        // 頂点情報の抽出
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            aiVector3D pos = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];
            vertex.position = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
            vertex.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
            distModel.vertices.push_back(vertex);
        }

        // インデックス情報の抽出
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                distModel.indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
            }
        }
    }
}

ModelManager::ModelManager(ID3D11Device1& device)
{
    m_models = std::vector<Model*>();
    m_device = &device;
}

Model* ModelManager::AddModel(string path)
{
    Model* model = new Model();
    LoadModel(*model, path);
    model->CreateBuffers(*m_device);
    m_models.push_back(model);
    return model;
}

void ModelManager::RemoveModel(int index)
{
}

Model& ModelManager::GetModel(int index)
{
    Model* model = m_models[index];
    if (model == nullptr) {
        throw std::exception("ModelManager::GetModel() : model is nullptr.");
    }

    return *model;
}

void ModelManager::DrawAll(ID3D11DeviceContext& deviceContext, ConstantBuffer& constantBufferDisc, ID3D11Buffer& constantBuffer)
{
    int i = 0;
    for (Model* model : m_models) {
        Draw(i, deviceContext, constantBufferDisc, constantBuffer);
        i++;
    }
}

void ModelManager::Draw(int index, ID3D11DeviceContext& deviceContext, ConstantBuffer& constantBufferDisc, ID3D11Buffer& constantBuffer)
{
    Model* model = m_models[index];
    if (model == nullptr) {
        throw std::exception("ModelManager::GetModel() : model is nullptr.");
    }

    XMStoreFloat4x4(&constantBufferDisc.world, XMMatrixTranspose(model->GetTransform().GetWorldMatrix()));
    deviceContext.UpdateSubresource(&constantBuffer, 0, nullptr, &constantBufferDisc, 0, 0);

    //  とりあえず受け取る
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    deviceContext.IASetVertexBuffers(0, 1, model->vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext.IASetIndexBuffer(model->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext.DrawIndexed(model->IndiceCount, 0, 0);
}

void ModelManager::UpdateAll()
{
}

void ModelManager::Update(int index)
{
}
