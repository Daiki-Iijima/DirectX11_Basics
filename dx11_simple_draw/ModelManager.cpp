#include "Common/pch.h"
#include "ModelManager.h"

#pragma comment(lib, "DirectXTex.lib")
#include <DirectXTex.h>
#include "HitDetection/SphereHitDetection.h"
#include "HitDetection/HitDetectionDebugView.h"

using namespace DirectX;

HRESULT CreateTextureFromPath(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& path, ID3D11ShaderResourceView** textureView) {
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratchImage);
    if (FAILED(hr)) {
        return hr;
    }

    hr = DirectX::CreateShaderResourceView(device, scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), textureView);
    return hr;
}

void CreateTexture(ID3D11Device* device,aiColor3D diffuseColor, ID3D11ShaderResourceView** textureView) {
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    unsigned int color = (255 << 24) | ((int)(diffuseColor.b * 255) << 16) | ((int)(diffuseColor.g * 255) << 8) | (int)(diffuseColor.r * 255);
    initData.pSysMem = &color;
    initData.SysMemPitch = sizeof(unsigned int);

    ID3D11Texture2D* generateTexture = nullptr;
    device->CreateTexture2D(&texDesc, &initData, &generateTexture);

    device->CreateShaderResourceView(generateTexture, nullptr, textureView);
    generateTexture->Release();
}

void ModelManager::LoadModel(std::vector<Model*>* models, string modelPath) {
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

    //  メッシュの読み込み
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        Model* model = new Model();
        models->push_back(model);
        Mesh meshComponent = model->GetMesh();
        
        //  名前の設定
        model->SetName(mesh->mName.C_Str());

        // 頂点情報の抽出
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            aiVector3D pos = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];
            vertex.position = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
            vertex.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);

            //  テクスチャ座標の取得
            if (mesh->HasTextureCoords(0)) {
                aiVector3D texcoord = mesh->mTextureCoords[0][i];
                vertex.texcoord = DirectX::XMFLOAT2(texcoord.x, texcoord.y);
            }
            else {
                //  テクスチャ座標がない場合は0で埋める
                vertex.texcoord = DirectX::XMFLOAT2(0.0f, 0.0f);
            }

            meshComponent.GetVertices()->push_back(vertex);
        }

        // インデックス情報の抽出
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                meshComponent.GetIndices()->push_back(static_cast<unsigned short>(face.mIndices[j]));
            }
        }

        //  マテリアル情報の抽出
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        //  テクスチャの読み込み
        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
            // モデルパスからディレクトリを抽出
            std::string modelDirectory = modelPath;
            size_t lastSlashPos = modelDirectory.find_last_of("/\\");
            if (lastSlashPos != std::string::npos) {
                modelDirectory = modelDirectory.substr(0, lastSlashPos + 1);
            }
            else {
                modelDirectory = ""; // ディレクトリが見つからない場合、カレントディレクトリを使用
            }

            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, i, &str);
            //  テクスチャの読み込み
            //  パスを出力してみる
            std::wstring path(str.C_Str(), str.C_Str() + str.length);
            OutputDebugString(std::wstring(path.begin(), path.end()).c_str());

            //  読み込んだテクスチャ情報
            ID3D11ShaderResourceView* textureView = nullptr;

            //  パスを正式なパスに変換
            // std::string型のmodelDirectoryをstd::wstringに変換
            std::wstring wModelDirectory(modelDirectory.begin(), modelDirectory.end());

            // パスの結合
            std::wstring fullPath = wModelDirectory + std::wstring(str.C_Str(), str.C_Str() + str.length);

            // デバッグ出力
            OutputDebugString(fullPath.c_str());

            //  テクスチャの生成
            CreateTextureFromPath(m_device,m_deviceContext,fullPath,&textureView);

            //  モデルクラスに保存
            model->AddTexture(textureView);
        }

        if (model->GetTextureCount() <= m) {
            //  テクスチャがない場合は白色のテクスチャを生成
            ID3D11ShaderResourceView* textureView = nullptr;
            aiColor3D color(0.f, 0.f, 0.f);
            //  色が取得できた場合はその色をテクスチャにする
            if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
                CreateTexture(m_device, color, &textureView);
            }
            //  色が取得できなかった場合は白色
            else {
                CreateTexture(m_device, color, &textureView);
            }
            model->AddTexture(textureView);
        }
    }
}


ModelManager::ModelManager(ID3D11Device1& device, ID3D11DeviceContext& deviceContext)
{
    m_models = std::vector<Model*>();

    m_device = &device;
    m_deviceContext = &deviceContext;
}

std::vector<Model*>* ModelManager::CreateModelFromObj(string path)
{
    std::vector<Model*>* models = new std::vector<Model*>();
    LoadModel(models, path);

    for(Model* model : *models){
        //  当たり判定の生成
        SphereHitDetection* hitDetection = new SphereHitDetection(model);

        // 当たり判定開始時のコールバックを設定
        hitDetection->SetOnHitStart([](BaseHitDetection* other) {
            OutputDebugStringW(L"当たり判定開始\n");
            });

        // 当たり判定持続中のコールバックを設定
        hitDetection->SetOnHitStay([](BaseHitDetection* other) {
            });

        // 当たり判定終了時のコールバックを設定
        hitDetection->SetOnHitExit([](BaseHitDetection* other) {
            OutputDebugStringW(L"当たり判定終了\n");
            });

        model->SetHitDetection(hitDetection);
        model->GetMesh().CreateBuffer(*m_device);

        //  当たり判定を持っているオブジェクトの当たり判定を収集
        m_hitDetections = std::vector<BaseHitDetection*>();
        for (Model* model : m_models) {
            BaseHitDetection* hitDetection = model->GetHitDetection();
            if (hitDetection != nullptr) {
                m_hitDetections.push_back(hitDetection);
            }
        }
        m_models.push_back(model);
    };

    return models;
}

void ModelManager::DrawUIAll()
{
    int i = 0;
    for (Model* model : m_models) {
        DrawUI(i);
        i++;
    }
}

void ModelManager::DrawUI(int index) {
    Model* model = m_models[index];
    if (model == nullptr) {
        throw std::exception("ModelManager::GetModel() : model is nullptr.");
    }
    if (ImGui::CollapsingHeader(model->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        for (IComponentUIDebugView* componentView : model->GetComponentUIDebugViews()) {
            componentView->ComponentUIRender();
        }
    }
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

void ModelManager::DrawAll(ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer)
{
    int i = 0;
    for (Model* model : m_models) {
        Draw(i, deviceContext, vsConstantBufferDisc, vsConstantBuffer);
        i++;
    }
}

void ModelManager::Draw(int index, ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer)
{
    Model* model = m_models[index];
    if (model == nullptr) {
        throw std::exception("ModelManager::GetModel() : model is nullptr.");
    }

    for (Model* model : m_models) {
        //  頂点用の定数バッファのワールド座標更新
        XMStoreFloat4x4(&vsConstantBufferDisc.world, XMMatrixTranspose(model->GetTransform().GetWorldMatrix()));
        deviceContext.UpdateSubresource(&vsConstantBuffer, 0, nullptr, &vsConstantBufferDisc, 0, 0);

        //  テクスチャの設定
        if (model->GetTexture(0) != nullptr) {
            ID3D11ShaderResourceView* textureView = model->GetTexture(0);
            ID3D11ShaderResourceView* views[] = { textureView };
            deviceContext.PSSetShaderResources(0, 1, views);
        }

        model->GetMesh().Draw(deviceContext);
    }
}

void ModelManager::UpdateAll()
{
    int i = 0;
    for (Model* model : m_models) {
        Update(i);
        i++;
    }
}

void ModelManager::Update(int index)
{
    Model* model = m_models[index];
    BaseHitDetection* hitDitection = model->GetHitDetection();
    if (hitDitection != nullptr && m_hitDetections.size() >= 2) {
        hitDitection->HitCheck(m_hitDetections);
    }

    //DirectX::XMVECTOR rot = model->GetTransform().GetDegressRotation();
    //float y = XMVectorGetY(rot);
    //float x = XMVectorGetX(rot);
    //y += 1.f;
    //x += 1.f;
    //model->GetTransform().SetDegressRotation(x,y,0);
}
