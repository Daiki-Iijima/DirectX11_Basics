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
            DirectX::XMVECTOR direction = bullet->direction; // ���˃x�N�g�����擾
            DirectX::XMVECTOR normalVector = DirectX::XMVectorZero(); // �@���x�N�g���̏�����
            bool hitWall = false; // �ǂɓ����������ǂ����̃t���O

            if (DirectX::XMVectorGetX(position) > 15 || DirectX::XMVectorGetX(position) < -15 ||
                DirectX::XMVectorGetZ(position) > 15 || DirectX::XMVectorGetZ(position) < -15) {
                hitWall = true; // �ǂɓ�������
                if (DirectX::XMVectorGetX(position) > 15) {
                    normalVector = DirectX::XMVectorSet(-1, 0, 0, 0); // �E�̕�
                }
                else if (DirectX::XMVectorGetX(position) < -15) {
                    normalVector = DirectX::XMVectorSet(1, 0, 0, 0); // ���̕�
                }
                else if (DirectX::XMVectorGetZ(position) > 15) {
                    normalVector = DirectX::XMVectorSet(0, 0, -1, 0); // �O�̕�
                }
                else if (DirectX::XMVectorGetZ(position) < -15) {
                    normalVector = DirectX::XMVectorSet(0, 0, 1, 0); // ���̕�
                }

                // �ǂɓ��������ꍇ�A���˃x�N�g�����v�Z���ĐV���������Ƃ���
                if (hitWall) {
                    DirectX::XMVECTOR reflectionVector = DirectX::XMVector3Reflect(direction, normalVector);
                    bullet->direction = reflectionVector; // �V����������ݒ�
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

            //  �e�̍\���̂̐���
            auto bullet = std::make_shared<Bullet>();
            bullet->model = bulletModel.get();
            bullet->position = m_worldPosition + forward * 0.75f;    //  �^���N�̈ʒu�̏����O
            bullet->direction = forward;
            m_bullets.push_back(bullet);

            //  �e�̓����蔻��̒ǉ�
            //  ���̓����蔻��́A�ǂɂ͗����Ȃ�
            //  ����SphereHitDetection�������Ă��郂�f���ɂ���������Ȃ�
            auto hitDetection = std::make_shared<SphereHitDetection>(bulletModel, m_pModelManager);
            hitDetection->SetOnHitStart([this, bullet](BaseHitDetection* other) {
                m_pModelManager->EraseModel(other->GetModel());
                auto it = std::find(m_bullets.begin(), m_bullets.end(), bullet);
                m_bullets.erase(it);
                });
            bulletModel->AddComponent(std::move(hitDetection));
        }
        // ���݂̏�Ԃ�O��̏�ԂƂ��ĕێ�
        wasSpaceKeyDown = isSpaceKeyDown;

        ModelsUpdateTransform(m_worldPosition, m_worldRotation);
        CameraUpdateTransform(m_worldPosition, m_worldRotation);
        BulletsUpdateTransfom(5.f * elapsedTime);
        BulletsWallCheck();
    }

private:
    //  �^���N�S�̂̍��W
    DirectX::XMVECTOR m_worldPosition;
    DirectX::XMVECTOR m_worldRotation;

    //  �^���N�̃��f��(����)
    std::vector<std::shared_ptr<Model>> m_tankModels;
    //  �J����
    Camera* m_camera;

    //  ���f���}�l�[�W���[
    ModelManager* m_pModelManager;

    struct Bullet {
        Model* model;
        DirectX::XMVECTOR direction;
        DirectX::XMVECTOR position;
    };
    //  ���������e�̃��X�g
    std::vector<std::shared_ptr<Bullet>> m_bullets;

    float m_moveSpeed = 1.0f;
    float m_rotateSpeed = 45.0f;

    //  �X�y�[�X�L�[���O�̃t���[���ŉ�����Ă�����
    bool wasSpaceKeyDown = false;
};

