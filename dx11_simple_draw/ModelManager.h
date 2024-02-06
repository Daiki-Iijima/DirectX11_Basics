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

    //  ���f���̒ǉ�
    std::vector<Model*>* CreateModelFromObj(string path);

    //  ���f���̍폜
    void RemoveModel(int index);


    //  ���f���̐�
    std::vector<Model*>& GetAllModels();

    //  ���f���̎擾
    Model& GetModel(int index);

    //  ���ׂẴ��f���̕`��
    void DrawAll(ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer);

    //  �w�肵�����f���̕`��
    void Draw(int index, ID3D11DeviceContext& deviceContext, VsConstantBuffer& vsConstantBufferDisc, ID3D11Buffer& vsConstantBuffer);

    void DrawUIAll();
    void DrawUI(int index);

    //  ���ׂẴ��f���̍X�V
    void UpdateAll();

    //  �w�肵�����f���̍X�V
    void Update(int index);

    void AddComponent(IComponent* component);

    //  ���f���̐�
    int GetModelCount() const {
        return m_models.size();
    }

private:
    //  ���f���̃��X�g
    std::vector<Model*> m_models;

    //  ���f���̓ǂݍ���
    void LoadModel(std::vector<Model*>* models, string modelPath);

    //  �f�o�C�X
    ID3D11Device1* m_device;
    ID3D11DeviceContext* m_deviceContext;
};
