#pragma once
#include "Model.h"
#include <DirectXMath.h>
#include "HitDetection/SphereHitDetection.h"

class TankModel
{
public:
    TankModel(std::vector<std::shared_ptr<Model>> tankModels,Camera* camera,ModelManager* modelManager) :
        m_tankModels(std::move(tankModels)),
        m_camera(camera),
        m_pModelManager(modelManager),
        m_worldPosition(DirectX::XMVectorSet(0, 0, 0, 0)),
        m_worldRotation(DirectX::XMVectorSet(0, 0, 0, 0))
    {};

    DirectX::XMVECTOR GetForwardVector() {
        float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
        DirectX::XMVECTOR forward = DirectX::XMVectorSet(
            sin(yAngle),
            0,
            cos(yAngle),
            0);
        return forward;
    }

    void ModelsUpdateTransform(DirectX::XMVECTOR worldPosition,DirectX::XMVECTOR worldRotation) {
        for (auto model : m_tankModels) {
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


        DirectX::XMVECTOR forward = GetForwardVector();
        DirectX::XMVECTOR position = m_camera->GetTransform().GetPosition();
        m_camera->GetTransform().SetPosition(worldPosition + -forward * 5 + DirectX::XMVectorSet(0, 2, 0, 0));
    }

    void BulletsUpdateTransfom(float bulletSpeed) {
        for (auto bullet : m_bullets) {
            bullet->position += bullet->direction * bulletSpeed;
            bullet->model->GetTransform().SetPosition(bullet->position);
        }
    }

    void BulletsWallCheck() {
        for (auto bullet : m_bullets) {
            DirectX::XMVECTOR position = bullet->position;
            DirectX::XMVECTOR direction = bullet->direction; // 入射ベクトルを取得
            DirectX::XMVECTOR normalVector = DirectX::XMVectorZero(); // 法線ベクトルの初期化
            bool hitWall = false; // 壁に当たったかどうかのフラグ

            if (DirectX::XMVectorGetX(position) > 15 || DirectX::XMVectorGetX(position) < -15 ||
                DirectX::XMVectorGetZ(position) > 15 || DirectX::XMVectorGetZ(position) < -15) {
                hitWall = true; // 壁に当たった
                if (DirectX::XMVectorGetX(position) > 15) {
                    normalVector = DirectX::XMVectorSet(-1, 0, 0, 0); // 右の壁
                }
                else if (DirectX::XMVectorGetX(position) < -15) {
                    normalVector = DirectX::XMVectorSet(1, 0, 0, 0); // 左の壁
                }
                else if (DirectX::XMVectorGetZ(position) > 15) {
                    normalVector = DirectX::XMVectorSet(0, 0, -1, 0); // 前の壁
                }
                else if (DirectX::XMVectorGetZ(position) < -15) {
                    normalVector = DirectX::XMVectorSet(0, 0, 1, 0); // 後ろの壁
                }

                // 壁に当たった場合、反射ベクトルを計算して新しい方向とする
                if (hitWall) {
                    DirectX::XMVECTOR reflectionVector = DirectX::XMVector3Reflect(direction, normalVector);
                    bullet->direction = reflectionVector; // 新しい方向を設定
                }
            }
        }
    }

    void Update(float elapsedTime) {

        DirectX::XMVECTOR forward = GetForwardVector();
        if (GetAsyncKeyState('D') & 0x8000) {
            m_worldRotation += DirectX::XMVectorSet(0, m_rotateSpeed * elapsedTime, 0, 0);
        }
        else if (GetAsyncKeyState('A') & 0x8000) {
            m_worldRotation += DirectX::XMVectorSet(0, -m_rotateSpeed * elapsedTime, 0, 0);
        }
        else if (GetAsyncKeyState('W') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            m_worldPosition += forward * m_moveSpeed * elapsedTime;
        }
        else if (GetAsyncKeyState('W') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            m_worldPosition += forward * m_moveSpeed * elapsedTime;
        }
        else if (GetAsyncKeyState('S') & 0x8000) {
            float yAngle = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(m_worldRotation));
            m_worldPosition -= forward * m_moveSpeed * elapsedTime;
        }

        bool isSpaceKeyDown = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if (isSpaceKeyDown && !wasSpaceKeyDown) {
            auto models = m_pModelManager->CreateModelFromObj("Models/Bullet.obj");
            auto& bulletModel = models.at(0);

            //  弾の構造体の生成
            auto bullet = std::make_shared<Bullet>();
            bullet->model = bulletModel.get();
            bullet->position = m_worldPosition + forward * 0.75f;    //  タンクの位置の少し前
            bullet->direction = forward;
            m_bullets.push_back(bullet);

            //  弾の当たり判定の追加
            //  この当たり判定は、壁には利かない
            //  同じSphereHitDetectionを持っているモデルにしか当たらない
            auto hitDetection = std::make_shared<SphereHitDetection>(bulletModel, m_pModelManager);
            hitDetection->SetOnHitStart([this, bullet](BaseHitDetection* other) {
                m_pModelManager->EraseModel(other->GetModel());
                auto it = std::find(m_bullets.begin(), m_bullets.end(), bullet);
                m_bullets.erase(it);
                });
            bulletModel->AddComponent(std::move(hitDetection));
        }
        // 現在の状態を前回の状態として保持
        wasSpaceKeyDown = isSpaceKeyDown;

        ModelsUpdateTransform(m_worldPosition, m_worldRotation);
        CameraUpdateTransform(m_worldPosition, m_worldRotation);
        BulletsUpdateTransfom(5.f * elapsedTime);
        BulletsWallCheck();
    }

private:
    //  タンク全体の座標
    DirectX::XMVECTOR m_worldPosition;
    DirectX::XMVECTOR m_worldRotation;

    //  タンクのモデル(複数)
    std::vector<std::shared_ptr<Model>> m_tankModels;
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
    std::vector<std::shared_ptr<Bullet>> m_bullets;

    float m_moveSpeed = 1.0f;
    float m_rotateSpeed = 45.0f;

    //  スペースキーが前のフレームで押されていたか
    bool wasSpaceKeyDown = false;
};

