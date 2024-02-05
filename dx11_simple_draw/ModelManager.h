#pragma once
#include <Game.h>
#include "Model.h"

struct VsConstantBuffer {
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

class ModelManager
{
public:
    ModelManager(ID3D11Device1& device, ID3D11DeviceContext& deviceContext);

    //  モデルの追加
    std::vector<Model*>* CreateModelFromObj(string path);

    //  モデルの削除
    void RemoveModel(int index);

    //  モデルの取得
    Model& GetModel(int index);

    //  すべてのモデルの描画
    void DrawAll(ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer);

    //  指定したモデルの描画
    void Draw(int index, ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer);

    void DrawUIAll();
    void DrawUI(int index);

    //  すべてのモデルの更新
    void UpdateAll();

    //  指定したモデルの更新
    void Update(int index);

    //  モデルの数
    int GetModelCount() const {
        return m_models.size();
    }

private:
    //  モデルのリスト
    std::vector<Model*> m_models;

    //  モデルの読み込み
    void LoadModel(std::vector<Model*>* models, string modelPath);

    //  デバイス
    ID3D11Device1* m_device;
    ID3D11DeviceContext* m_deviceContext;

    //  当たり判定のリスト
    std::vector<BaseHitDetection*> m_hitDetections;
};
