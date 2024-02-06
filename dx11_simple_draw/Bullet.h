#pragma once
#include "Model.h"
#include "ModelManager.h"

class Bullet {
public:
    Bullet(ModelManager* modelManager) {
        mp_modelManager = modelManager;
    }

    void SpownBullet(DirectX::XMVECTOR spownPosition) {
        std::vector<Model*>* models = mp_modelManager->CreateModelFromObj("Models/bullet.obj");
        //  現状の弾はメッシュが一つしかないので、最初のモデルを取得する
        mp_model = models->at(0);
        mp_model->GetTransform().SetPosition(spownPosition);
    }

    void Update(DirectX::XMVECTOR direction, float elapsedTime) {
        mp_model->GetTransform().SetPosition(mp_model->GetTransform().GetPosition() + direction * elapsedTime);
    }

private:
    Model* mp_model;
    ModelManager* mp_modelManager;
};
