#pragma once
#include <Game.h>
#include "Model.h"

struct ConstantBuffer {
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

class ModelManager
{
public:
    ModelManager(ID3D11Device1& device, ID3D11DeviceContext& deviceContext);

    //  ���f���̒ǉ�
    Model* AddModel(string path, string modelName = "");

    //  ���f���̍폜
    void RemoveModel(int index);

    //  ���f���̎擾
    Model& GetModel(int index);

    //  ���ׂẴ��f���̕`��
    void DrawAll(ID3D11DeviceContext& deviceContext, ConstantBuffer& constantBufferDisc, ID3D11Buffer& constantBuffer);

    //  �w�肵�����f���̕`��
    void Draw(int index, ID3D11DeviceContext& deviceContext, ConstantBuffer& constantBufferDisc, ID3D11Buffer& constantBuffer);

    void DrawUIAll();
    void DrawUI(int index);

    //  ���ׂẴ��f���̍X�V
    void UpdateAll();

    //  �w�肵�����f���̍X�V
    void Update(int index);

    //  ���f���̐�
    int GetModelCount() const {
        return m_models.size();
    }

private:
    //  ���f���̃��X�g
    std::vector<Model*> m_models;

    //  ���f���̓ǂݍ���
    void LoadModel(Model& model, string path);

    //  �f�o�C�X
    ID3D11Device1* m_device;
    ID3D11DeviceContext* m_deviceContext;

    //  �����蔻��̃��X�g
    std::vector<BaseHitDetection*> m_hitDetections;
};
