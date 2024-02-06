#pragma once

#include<DirectXMath.h>
#include "HitDetection/BaseHitDetection.h"
#include "Model.h"

using namespace DirectX;

struct BoundingSphere {
    XMVECTOR center;
    float radius;
};

class SphereHitDetection : public BaseHitDetection
{
public:
    SphereHitDetection(Model* model,ModelManager* modelManager);

    //  �����蔻��
    void HitCheck(vector<Model*>& allModels) override;

    BoundingSphere GetBoundingSphere();

    //  Getter
    float GetRadius() {
        return m_radius;
    }

private:
    //  BoundingSphere�̔��a�̌v�Z
    float CalulateBoundingSphereRadius(const XMVECTOR& center, const std::vector<Vertex>& vertices);

    //  ���a
    float m_radius;

    //  �������Ă���I�u�W�F�N�g���X�g
    vector<BaseHitDetection*> m_hitObjects;
};

