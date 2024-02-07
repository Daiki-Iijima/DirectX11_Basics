#pragma once
#include "Model.h"
#include <DirectXMath.h>

class TankModel
{
public:
    TankModel(std::vector<Model*>* tankModels,Camera* camera,ModelManager* modelManager) :
        m_tankModels(tankModels),
        m_camera(camera),
        m_pModelManager(modelManager),
        m_worldPosition(DirectX::XMVectorSet(0, 0, 0, 0)),
        m_worldRotation(DirectX::XMVectorSet(0, 0, 0, 0)),
        m_bullets(std::vector<Bullet*>())
    {};

    void ModelsUpdateTransform(DirectX::XMVECTOR worldPosition,DirectX::XMVECTOR worldRotation) {
        for (auto model : *m_tankModels) {
            model->GetTransform().SetPosition(DirectX::XMVectorSet(
                DirectX::XMVectorGetX(worldPosition),
                DirectX::XMVectorGetY(worldPosition),
                DirectX::XMVectorGetZ(worldPosition),
                0));

            model->GetTransform().SetDegreesRotation(
                DirectX::XMVectorGetX(worldRotation),
                DirectX::XMVectorGetY(worldRotation),
                DirectX::XMVectorGetZ(worldRotation));
        }
    }

    void CameraUpdateTransform(DirectX::XMVECTOR worldPosition, DirectX::XMVECTOR worldRotation) {
        worldRotation += DirectX::XMVectorSet(10.0f, 0, 0, 0);

        m_camera->GetTransform().SetDegreesRotation(
            DirectX::XMVectorGetX(worldRotation),
            DirectX::XMVectorGetY(worldRotation),
            DirectX::XMVectorGetZ(worldRotation));


        float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(worldRotation));
        DirectX::XMVECTOR forward = DirectX::XMVectorSet(
            sin(yAngle),
            0,
            cos(yAngle),
            0);

        DirectX::XMVECTOR position = m_camera->GetTransform().GetPosition();
        m_camera->GetTransform().SetPosition(worldPosition + -forward * 5 + DirectX::XMVectorSet(0, 2, 0, 0));
    }

    void BulletsUpdateTransfom(float bulletSpeed) {
        for (auto bullet : m_bullets) {
            bullet->position += bullet->direction * bulletSpeed;
            bullet->model->GetTransform().SetPosition(bullet->position);
        }
    }

    void Update(float elapsedTime) {

        if (GetAsyncKeyState('D') & 0x8000) {
            m_worldRotation += DirectX::XMVectorSet(0, m_rotateSpeed * elapsedTime, 0, 0);
        }
        else if (GetAsyncKeyState('A') & 0x8000) {
            m_worldRotation += DirectX::XMVectorSet(0, -m_rotateSpeed * elapsedTime, 0, 0);
        }
        else if (GetAsyncKeyState('W') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            DirectX::XMVECTOR forward = DirectX::XMVectorSet(
                sin(yAngle),
                0,
                cos(yAngle),
                0);

            m_worldPosition += forward * m_moveSpeed * elapsedTime;
        }
        else if (GetAsyncKeyState('W') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            DirectX::XMVECTOR forward = DirectX::XMVectorSet(
                sin(yAngle),
                0,
                cos(yAngle),
                0);

            m_worldPosition += forward * m_moveSpeed * elapsedTime;
        }
        else if (GetAsyncKeyState('S') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            DirectX::XMVECTOR forward = DirectX::XMVectorSet(
                sin(yAngle),
                0,
                cos(yAngle),
                0);

            m_worldPosition -= forward * m_moveSpeed * elapsedTime;
        }

        bool isSpaceKeyDown = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if (isSpaceKeyDown && !wasSpaceKeyDown) {
            Model* bulletModel = m_pModelManager->CreateModelFromObj("Models/Bullet.obj")->at(0);
            Bullet* bullet = new Bullet();
            bullet->model = bulletModel;
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            DirectX::XMVECTOR forward = DirectX::XMVectorSet(
                sin(yAngle),
                0,
                cos(yAngle),
                0);
            bullet->position = m_worldPosition + forward * 0.75f;    //  タンクの位置の少し前
            bullet->direction = forward;
            m_bullets.push_back(bullet);
        }
        // 現在の状態を前回の状態として保持
        wasSpaceKeyDown = isSpaceKeyDown;

        ModelsUpdateTransform(m_worldPosition, m_worldRotation);
        CameraUpdateTransform(m_worldPosition, m_worldRotation);
        BulletsUpdateTransfom(2.f * elapsedTime);
    }

private:
    //  タンク全体の座標
    DirectX::XMVECTOR m_worldPosition;
    DirectX::XMVECTOR m_worldRotation;

    //  タンクのモデル(複数)
    std::vector<Model*>* m_tankModels;
    //  カメラ
    Camera* m_camera;

    //  モデルマネージャー
    ModelManager* m_pModelManager;

    struct Bullet {
        Model* model;
        DirectX::XMVECTOR direction;
        DirectX::XMVECTOR position;
    };
    //  生成した弾のリスト
    std::vector<Bullet*> m_bullets;

    float m_moveSpeed = 1.0f;
    float m_rotateSpeed = 45.0f;

    //  スペースキーが前のフレームで押されていたか
    bool wasSpaceKeyDown = false;
};

